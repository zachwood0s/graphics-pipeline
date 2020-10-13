#pragma once
#include "vec3d.h"
#include "Material.h"
#include "WorldView.h"

class Light
{
private:
	Vec3d center;

public:
	WorldView * shadowMap;
	static constexpr float SHADOW_EPS = 0.05f;
	Light(Vec3d center, int shadowResX, int shadowResY, float fov);
	~Light();

	float constant = 0.8f;
	float linear = 0.0045f;
	float quadradic = 0.000075f; // Really powerful point lighs

	float ambient = .1f;
	float diffuse = .8f;
	float specular = 1.0f;


	Vec3d LightPixel(Vec3d point, Vec3d curColor, Vec3d normalVector, Vec3d viewDir, Material mat) const;
	bool IsInShadow(Vec3d point);
	void UpdateShadowMap(Scene &scene);
	void SetCenter(Vec3d center);
	Vec3d GetCenter() const;
	void SetAttenuation(float constant, float linear, float quadradic);
};

