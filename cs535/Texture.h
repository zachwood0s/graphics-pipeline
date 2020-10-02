#pragma once

#include "framebuffer.h"

class Texture : public FrameBuffer
{
public:

	bool isMipmap;
	unsigned int mipW;
	unsigned int mipH;

	void SetAsMipmap(int mipW, int mipH);

	Vec3d GetTexVal(Vec3d texCoords);
	Vec3d GetTexVal(int s, int t);
};

