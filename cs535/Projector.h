#pragma once
#include "framebuffer.h"
#include "Light.h"

class Scene;
class Projector : public Light
{
public:
	Projector(Vec3d center, int w, int h, float hfov, TEX_HANDLE tex);

	bool GetColorAt3dPoint(Scene &scene, Vec3d point, Vec3d &color);

	TEX_HANDLE texture;
};

