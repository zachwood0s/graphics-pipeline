#define _USE_MATH_DEFINES
#include <math.h>
#include "stdafx.h"

#include "ppc.h"

#include "Matrix3d.h"

PPC::PPC(float hfov, int _w, int _h) {

	w = _w;
	h = _h;
	C = Vec3d(0.0f, 0.0f, 0.0f);
	a = Vec3d(1.0f, 0.0f, 0.0f);
	b = Vec3d(0.0f, -1.0f, 0.0f);
	float hfovd = hfov / 180.0f * M_PI;
	c = Vec3d(-(float)w / 2.0f, (float)h / 2, -(float)w / (2 * tanf(hfovd / 2.0f)));

}

int PPC::Project(Vec3d P, Vec3d &p) {

	Matrix3d M;
	M.SetColumn(0, a);
	M.SetColumn(1, b);
	M.SetColumn(2, c);
	Vec3d q = M.Inverted()*(P - C);
	float w = q[2];
	if (w <= 0.0f)
		return 0;

	p[0] = q[0] / q[2];
	p[1] = q[1] / q[2];
	p[2] = 1.0f / w;
	return 1;
}

Vec3d PPC::UnProject(Vec3d p) {

	Vec3d ret;
	ret = C + (a*p[0] + b*p[1] + c)/p[2];
	return ret;

}


void PPC::TranslateRightLeft(float tstep) {

	Vec3d rightDir = a.Normalized();
	C = C + rightDir*tstep;

}

void PPC::TranslateFrontBack(float tstep) {

	Vec3d tDir = GetViewDirection();
	C = C + tDir*tstep;
}

void PPC::TranslateUpDown(float tStep) 
{
	Vec3d upDir = b.Normalized() * -1;
	C = C + upDir * tStep;
}

void PPC::PanLeftRight(float rstep)
{
	Vec3d bDir = b.Normalized() * -1.0f;
	a = a.Rotate(bDir, rstep);
	c = c.Rotate(bDir, rstep);
}

void PPC::TiltUpDown(float rstep)
{
	Vec3d aDir = a.Normalized();
	b = b.Rotate(aDir, rstep);
	c = c.Rotate(aDir, rstep);
}

void PPC::Roll(float rstep)
{
	Vec3d vdDir = GetViewDirection();
	a = a.Rotate(vdDir, rstep);
	b = b.Rotate(vdDir, rstep);
	c = c.Rotate(vdDir, rstep);
}

void PPC::SetPose(Vec3d newEye, Vec3d lookAtPoint, Vec3d upGuidance)
{
	Vec3d newViewDirection = (lookAtPoint - newEye).Normalized();
	Vec3d newa = (newViewDirection ^ upGuidance).Normalized();
	Vec3d newb = (newViewDirection ^ newa).Normalized();
	Vec3d principal = GetPrincipal();
	Vec3d newc = newViewDirection * GetFocalLength() - newa * principal[0] - newb * principal[1];

	// commit changes to camera
	a = newa;
	b = newb;
	c = newc;
	C = newEye;
}

void PPC::Interpolate(PPC* ppc0, PPC* ppc1, int currStep, int stepCount)
{
	PPC &ppc = *this;
	float scf = (float)currStep / (float)(stepCount - 1);

	Vec3d newC = Vec3d::Interpolate(ppc0->C, ppc1->C, scf); 

	Vec3d sLookAt = ppc0->C + ppc0->GetViewDirection();
	Vec3d dLookAt = ppc1->C + ppc1->GetViewDirection();
	Vec3d newLookAt = Vec3d::Interpolate(sLookAt, dLookAt, scf);

	Vec3d sUp = ppc0->b * -1;
	Vec3d dUp = ppc1->b * -1;
	Vec3d newUp = Vec3d::Interpolate(sUp, dUp, scf).Normalized();

	ppc.SetPose(newC, newLookAt, newUp);
}

float PPC::GetFocalLength()
{
	return GetViewDirection() * c;
}

void PPC::SetFocalLength(float zoom)
{
	Vec3d viewDirection = GetViewDirection();
	float newFocal = GetFocalLength() * zoom;
	Vec3d principal = GetPrincipal();
	Vec3d newc = viewDirection * newFocal - a * principal[0] - b * principal[1];

	c = newc;
}

Vec3d PPC::GetPrincipal()
{
	Vec3d pp;
	pp[0] = (c * -1) * a.Normalized() / a.Length();
	pp[1] = (c * -1) * b.Normalized() / b.Length();
	return pp;
}

Vec3d PPC::GetViewDirection()
{
	return (a ^ b).Normalized();
}

