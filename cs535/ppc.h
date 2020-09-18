#pragma once


#include "vec3d.h"

class WorldView;

class PPC {
public:
	Vec3d a, b, c, C;
	int w, h;
	PPC(float hfov, int _w, int _h);

	void SetPose(Vec3d newEye, Vec3d lookAtPoint, Vec3d upGuidance);
	void SetFocalLength(float focal);
	void ZoomFocalLength(float zoom);

	int Project(Vec3d P, Vec3d &p);
	Vec3d UnProject(Vec3d p);

	void TranslateRightLeft(float tstep);
	void TranslateFrontBack(float tstep);
	void TranslateUpDown(float tstep);

	void PanLeftRight(float rstep);
	void TiltUpDown(float rstep);
	void Roll(float rstep);

	float GetFocalLength();
	void Interpolate(PPC* ppc0, PPC* pp1, int currStep, int numSteps);
	Vec3d GetPrincipal();
	Vec3d GetViewDirection();

	void Visualize(WorldView *world, float vf, Vec3d colorBox, Vec3d colorPoint);
	void Visualize(WorldView *world, WorldView *cameraWorld, float vf);

};