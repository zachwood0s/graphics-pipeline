#pragma once

#include "vec3d.h"
#include "framebuffer.h"


class Material 
{
public:
	static Material DEFAULT(Vec3d color);
	Material(Vec3d color, float phong, float kSpec);
	Material(Vec3d color, float phong, float kSpec, TEX_HANDLE texture);

	Vec3d color;
	float phongExponent;
	float kSpecular;

	TEX_HANDLE texture = TEX_INVALID;
};

