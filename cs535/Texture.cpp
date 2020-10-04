#include <algorithm>

#include "Texture.h"
#include "framebuffer.h"
#include "vec3d.h"


Texture::Texture(int w, int h) : FrameBuffer(w, h), levels(0), mipH(h), mipW(w)
{
}

void Texture::SetAsMipmap(int _mipH)
{	
	mipH = _mipH;
	mipW = _mipH;
	levels = (int) log2(mipW);
	isMipmap = true;
}

std::tuple<Vec3d, float> Texture::GetTexVal(Vec3d texCoords, Vec3d texDeltas)
{
	float maxDelta = std::max(texDeltas[0] * mipW, texDeltas[1] * mipH);

	// Find the level of detail in both directions
	float lod = log2(maxDelta) + LOD_BIAS;

	// Clamp between the possible values for this texture and invert it so that 
	int lod0 = (int) clamp(lod, 0, levels);
	int lod1 = (int) clamp(lod0 + 1, 0, levels);

	// Trilinear interpolation: interpolate between the levels of detail
	auto[c1, a1] = GetTexVal(texCoords, lod0);
	auto[c2, a2] = GetTexVal(texCoords, lod1);
	Vec3d color = Vec3d::Interpolate(c1, c2, clamp(lod - lod0, 0.0f, 1.0f));
	return { color, std::min({a1, a2}) };
}

std::tuple<Vec3d, float> Texture::GetTexVal(Vec3d texCoords, int lod)
{
	// Find the width of the texture at the given level of detail
	int lodW = lod == 0 ? mipW : std::exp2(levels - lod);
	int lodH = lod == 0 ? mipH : lodW;

	// If this is the max detail, then there is no offset, otherwise add one width
	int offsetX = lod == 0 ? 0 : mipW;

	// Similar for the y, but if its not max level, sum up powers of 2 to find the starting y position
	int offsetY = lod == 0 ? 0 : mipH - (std::exp2(levels - lod + 1));

	float s = fmodf(texCoords[0] * lodW, lodW);
	float t = fmodf(texCoords[1] * lodH, lodH);

	int s0 = (int) s;
	int t0 = (int) t;
	int s1 = (int) clamp((float) (s0 + 1), 0.0f, (float) lodW - 1);
	int t1 = (int) clamp((float) (t0 + 1), 0.0f, (float) lodH - 1);

	// Interpolate the s val for both top and bottom
	auto[cTL, alphaTL] = GetTexVal(s0 + offsetX, t0 + offsetY);
	auto[cTR, alphaTR] = GetTexVal(s1 + offsetX, t0 + offsetY);
	auto[cBL, alphaBL] = GetTexVal(s0 + offsetX, t1 + offsetY);
	auto[cBR, alphaBR] = GetTexVal(s1 + offsetX, t1 + offsetY);
	
	Vec3d c1 = Vec3d::Interpolate(cTL, cTR, s - s0);
	Vec3d c2 = Vec3d::Interpolate(cBL, cBR, s - s0);

	// Interpolate the t val acroos top and bottom
	auto colorAlpha = Vec3d::Interpolate(c1, c2, t - t0);

	return { colorAlpha, std::min({ alphaTL, alphaTR, alphaBL, alphaBR }) };
}

std::tuple<Vec3d, float> Texture::GetTexVal(int s, int t)
{
	return { Vec3d::FromColor(Get(s, t)), GetTexAlpha(s, t) };
}

float Texture::GetTexAlpha(int s, int t)
{
	unsigned int color = Get(s, t);
	unsigned char *rgba = (unsigned char*)&color;

	return rgba[3] / 255.0f;

}


