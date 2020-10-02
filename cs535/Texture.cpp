#include "Texture.h"


void Texture::SetAsMipmap(int _mipW, int _mipH)
{	
	mipH = _mipH;
	mipW = _mipW;
	isMipmap = true;
}

Vec3d Texture::GetTexVal(Vec3d texCoords)
{
	int s = (int) (texCoords[0] * w) % w;
	int t = h - ((int) (texCoords[1] * h) % h);

	return GetTexVal(s, t);
}

