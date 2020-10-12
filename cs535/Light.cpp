#include <algorithm>

#include "Light.h"
#include "ppc.h"
#include "shaders.h"



Light::Light(Vec3d _center, int shadowResX, int shadowResY, float fov) : center(_center), shadowMap(new WorldView(shadowResX, shadowResY, fov, -1))
{
	
	shadowMap->GetPPC()->SetPose(center, Vec3d::ZEROS, Vec3d::YAXIS);
	shadowMap->shaders.push_back(Shaders::depthShading);
}

Light::~Light()
{
	delete shadowMap;
}


Vec3d Light::LightPixel(Vec3d point, Vec3d curColor, Vec3d normalVector, Vec3d viewDirection, Material m) const
{
	Vec3d lightVector = (center - point).Normalized();
	float kd = std::max(lightVector*normalVector, 0.0f);
	Vec3d reflected = (lightVector * -1).Reflect(normalVector);
	float kPhong = pow(std::max({ viewDirection * reflected, 0.0f }), m.phongExponent) * m.kSpecular;
	float distance = (center - point).Length();
	float attenuation = 1.0 / (constant + linear * distance + quadradic * (distance * distance));

	Vec3d diff = curColor * diffuse * kd * attenuation;
	Vec3d spec = curColor * specular * kPhong * attenuation;
	Vec3d amb = curColor * ambient * attenuation;
	
	return amb + diff + spec;
}

bool Light::IsInShadow(Vec3d point)
{
	PPC *ppc = shadowMap->GetPPC();
	FrameBuffer * fb = shadowMap->GetFB();

	Vec3d projected;

	if (!ppc->Project(point, projected))
		return false;

	if (projected[0] < 0 || projected[1] < 0 || projected[0] > fb->w || projected[1] > fb->h)
		return false;

	return fb->Farther((int)projected[0], (int)projected[1], projected[2] + SHADOW_EPS, false);

}

void Light::UpdateShadowMap(Scene &scene)
{
	shadowMap->Render(scene, true);
}

void Light::SetCenter(Vec3d _center)
{
	center = _center;
	shadowMap->GetPPC()->SetPose(center, Vec3d::ZEROS, Vec3d::YAXIS);
}

Vec3d Light::GetCenter() const
{
	return center;
}

void Light::SetAttenuation(float _constant, float _linear, float _quadradic)
{
	constant = _constant;
	linear = _linear;
	quadradic = _quadradic;
}
