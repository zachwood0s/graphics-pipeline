#include <algorithm>

#include "Texture.h"
#include "framebuffer.h"
#include "vec3d.h"


Texture::Texture(int w, int h) : FrameBuffer(w, h), levels(1)
{
}

void Texture::SetAsMipmap(int _mipW, int _mipH)
{	
	mipH = _mipH;
	mipW = _mipW;
	levels = (int) log2(mipW);
	isMipmap = true;
}

Vec3d Texture::GetTexVal(Vec3d texCoords, Vec3d texDeltas)
{
	float maxDelta = std::max(texDeltas[0] * w, texDeltas[1] * h);

	// Find the level of detail in both directions
	float lod = log2(maxDelta) + LOD_BIAS;

	// Clamp between the possible values for this texture and invert it so that 
	int lod0 = (int) clamp(lod, 0, levels);
	int lod1 = (int) clamp(lod0 + 1, 0, levels);

	// Trilinear interpolation: interpolate between the levels of detail
	Vec3d color = Vec3d::Interpolate(GetTexVal(texCoords, lod0), GetTexVal(texCoords, lod1), lod - lod0);
	return color;
}

Vec3d Texture::GetTexVal(Vec3d texCoords, int lod)
{
	// Find the width of the texture at the given level of detail
	int lodW = pow(2, levels - lod);
	int lodH = lodW;

	// If this is the max detail, then there is no offset, otherwise add one width
	int offsetX = lod == 0 ? 0 : mipW;

	// Similar for the y, but if its not max level, sum up powers of 2 to find the starting y position
	int offsetY = lod == 0 ? 0 : mipH - (pow(2, levels - lod + 1));

	float s = fmodf(texCoords[0] * lodW, lodW);
	float t = fmodf(texCoords[1] * lodH, lodH);
	int s0 = (int) s;
	int t0 = (int) t;
	int s1 = (int) clamp((float) (s0 + 1), 0.0f, (float) lodW);
	int t1 = (int) clamp((float) (t0 + 1), 0.0f, (float) lodH);

	// Interpolate the s val for both top and bottom
	Vec3d c1 = Vec3d::Interpolate(GetTexVal(s0 + offsetX, t0 + offsetY), GetTexVal(s1 + offsetX, t0 + offsetY), s - s0);
	Vec3d c2 = Vec3d::Interpolate(GetTexVal(s0 + offsetX, t1 + offsetY), GetTexVal(s1 + offsetX, t1 + offsetY), s - s0);

	// Interpolate the t val acroos top and bottom
	Vec3d color = Vec3d::Interpolate(c1, c2, t - t0);

	return color;
}

Vec3d Texture::GetTexVal(int s, int t)
{
	return Vec3d::FromColor(Get(s, t));
}


