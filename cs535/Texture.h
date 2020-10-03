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

	Vec3d GetTexVal(Vec3d texCoords, Vec3d texDeltas);
	Vec3d GetTexVal(Vec3d texCoords, int lod);
	Vec3d GetTexVal(int s, int t);
};

