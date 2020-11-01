#include "Material.h"
#include "scene.h"

Material::Material(Vec3d _color, float phong, float spec)
	: Material(_color, phong, spec, TEX_INVALID)
{
}

Material::Material(Vec3d _color, float phong, float spec, TEX_HANDLE tex)
	: color(_color), phongExponent(phong), kSpecular(spec), texture(tex), reflectivity(0.0f)
{
}

Material Material::DEFAULT(Vec3d color)
{
	return Material(color, 32, 0.5f);
}

std::tuple<Vec3d, float> Material::GetColor(Scene &scene, Vec3d point, Vec3d texDeltas)
{
	if (texture != TEX_INVALID)
	{
		auto& tex = scene.textures[texture];
		return tex->GetTexVal(point, texDeltas);
	}
	else
	{
		return { color, 1.0f };
	}
}
