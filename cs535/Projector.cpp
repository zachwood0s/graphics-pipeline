#include "Projector.h"
#include "scene.h"


Projector::Projector(Vec3d center, int w, int h, float hfov, TEX_HANDLE tex) : Light(center, w, h, hfov), texture(tex)
{
}

bool Projector::GetColorAt3dPoint(Scene &scene, Vec3d point, Vec3d &color)
{
	PPC *ppc = shadowMap->GetPPC();
	FrameBuffer * fb = shadowMap->GetFB();
	auto tex = scene.textures[texture];

	Vec3d projected;

	if (!ppc->Project(point, projected))
		return false;

	if (projected[0] < 0 || projected[1] < 0 || projected[0] > tex->w || projected[1] > tex->h)
		return false;

	auto[texColor, alpha] = tex->GetTexVal((int) projected[0], (int) projected[1]);

	if (fpclassify(alpha) == FP_ZERO)
		return false;

	color.Set(texColor);
	return true;

}

