#include "Material.h"

Material::Material(Vec3d _color, float phong, float spec)
	: Material(_color, phong, spec, TEX_INVALID)
{
}

Material::Material(Vec3d _color, float phong, float spec, TEX_HANDLE tex)
	: color(_color), phongExponent(phong), kSpecular(spec), texture(tex)
{
}

Material Material::DEFAULT(Vec3d color)
{
	return Material(color, 32, 0.5f);
}

