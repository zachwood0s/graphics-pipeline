#pragma once

#include "framebuffer.h"

class Texture : public FrameBuffer
{
public:
	static const int LOD_BIAS = 0;

	Texture(int w, int h);

	bool isMipmap;
	unsigned int mipW;
	unsigned int mipH;
	unsigned int levels;

	void SetAsMipmap(int h);

	std::tuple<Vec3d, float> GetTexVal(Vec3d texCoords, Vec3d texDeltas);
	std::tuple<Vec3d, float> GetTexVal(Vec3d texCoords, int lod);
	std::tuple<Vec3d, float> GetTexVal(int s, int t);
	float GetTexAlpha(int s, int t);
};

