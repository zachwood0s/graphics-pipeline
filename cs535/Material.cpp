#include "Material.h"

Material::Material(Vec3d _color, float phong, float spec)
	: color(_color), phongExponent(phong), kSpecular(spec)
{
}

Material Material::DEFAULT(Vec3d color)
{
	return Material(color, 32, 0.5f);
}

