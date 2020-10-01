#pragma once

#include "vec3d.h"


class Material 
{
public:
	static Material DEFAULT(Vec3d color);
	Material(Vec3d color, float phong, float kSpec);

	Vec3d color;
	float phongExponent;
	float kSpecular;

	//TEX_HANDLE texture = TEX_INVALID;
};

