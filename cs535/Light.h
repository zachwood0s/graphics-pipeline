#pragma once
#include "vec3d.h"
#include "Material.h"
#include "WorldView.h"

class Light
{
private:
	WorldView * shadowMap;
	Vec3d center;

public:
	const float SHADOW_EPS = 0.01f;
	Light(Vec3d center, int shadowResX, int shadowResY, float fov);
	~Light();


	Vec3d LightPixel(Vec3d point, Vec3d curColor, Vec3d normalVector, Vec3d viewDir, float ambient, Material mat) const;
	bool IsInShadow(Vec3d point);
	void UpdateShadowMap(Scene &scene);
	void SetCenter(Vec3d center);
	Vec3d GetCenter() const;
};

