#define _USE_MATH_DEFINES
#include <math.h>
#include <iostream>
#include <fstream>
#include "stdafx.h"

#include "ppc.h"
#include "Matrix3d.h"
#include "WorldView.h"

PPC::PPC(float hfov, int _w, int _h) 
{
	w = _w;
	h = _h;
	C = Vec3d(0.0f, 0.0f, 0.0f);
	a = Vec3d(1.0f, 0.0f, 0.0f);
	b = Vec3d(0.0f, -1.0f, 0.0f);
	float hfovd = hfov / 180.0f * (float) M_PI;
	c = Vec3d(-(float)w / 2.0f, (float)h / 2, -(float)w / (2 * tanf(hfovd / 2.0f)));

}

void PPC::ReadFromFile(std::string fname)
{
	std::ifstream file(fname);
	if (!file.is_open())
	{
		std::cerr << "INFO: cannot open file: " << fname.c_str() << std::endl;
		return;
	}

	file >> a >> b >> c >> C;


}

void PPC::WriteToFile(std::string fname)
{
	std::ofstream file (fname);

	if (!file.is_open())
	{
		std::cerr << "INFO: cannot open file: " << fname.c_str() << std::endl;
		return;
	}

	file << a << std::endl << b << std::endl << c << std::endl << C << std::endl;
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

	CacheInverted();
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

#pragma region Projection

bool PPC::Project(Vec3d P, Vec3d &p, bool invertW) 
{
	Vec3d q = invertedCached*(P - C);
	float w = q[2];
	if (w <= 0.0f)
		return false;

	p[0] = q[0] / q[2];
	p[1] = q[1] / q[2];

	if (invertW)
		p[2] = 1.0f / w;
	else
		p[2] = w;
	return true;
}

Vec3d PPC::UnProject(Vec3d p) 
{
	Vec3d ret;
	ret = C + (a*p[0] + b*p[1] + c)/p[2];
	return ret;

}

#pragma endregion

#pragma region Translation

void PPC::TranslateRightLeft(float tstep) 
{
	Vec3d rightDir = a.Normalized();
	C = C + rightDir*tstep;
}

void PPC::TranslateFrontBack(float tstep) 
{
	Vec3d tDir = GetViewDirection();
	C = C + tDir*tstep;
}

void PPC::TranslateUpDown(float tStep) 
{
	Vec3d upDir = b.Normalized() * -1;
	C = C + upDir * tStep;
}

#pragma endregion

#pragma region Pan/Tilt/Roll

void PPC::PanLeftRight(float rstep)
{
	Vec3d bDir = b.Normalized() * -1.0f;
	a = a.Rotate(bDir, rstep);
	c = c.Rotate(bDir, rstep);

	CacheInverted();
}

void PPC::TiltUpDown(float rstep)
{
	Vec3d aDir = a.Normalized();
	b = b.Rotate(aDir, rstep);
	c = c.Rotate(aDir, rstep);

	CacheInverted();
}

void PPC::Roll(float rstep)
{
	Vec3d vdDir = GetViewDirection();
	a = a.Rotate(vdDir, rstep);
	b = b.Rotate(vdDir, rstep);
	c = c.Rotate(vdDir, rstep);

	CacheInverted();
}

#pragma endregion

#pragma region Helpers

float PPC::GetFocalLength()
{
	return GetViewDirection() * c;
}

void PPC::ZoomFocalLength(float zoom)
{
	SetFocalLength(GetFocalLength() * zoom);
}

void PPC::SetFocalLength(float focal)
{
	Vec3d viewDirection = GetViewDirection();
	Vec3d principal = GetPrincipal();
	Vec3d newc = viewDirection * focal - a * principal[0] - b * principal[1];

	c = newc;
	CacheInverted();
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

void PPC::CacheInverted()
{
	Matrix3d M;
	M.SetColumn(0, a);
	M.SetColumn(1, b);
	M.SetColumn(2, c);
	invertedCached = M.Inverted();
}

void PPC::SetIntrinsicsHW() {

	glViewport(0, 0, w, h);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	float f = GetFocalLength();
	float hither = 0.25f;
	float yon = 1000.0f;
	float scalef = hither / f;
	float wf = a.Length() * w;
	float hf = b.Length() * h;
	glFrustum(-wf / 2.0f*scalef, wf / 2.0f*scalef, -hf / 2.0f*scalef,
		hf / 2.0f*scalef, hither, yon);
	glMatrixMode(GL_MODELVIEW); // default matrix mode

}

void PPC::SetExtrinsicsHW() {

	Vec3d eye, look, down;
	eye = C;
	look = C + (a^b)*100.0f;
	down = b.Normalized();
	glLoadIdentity();
	gluLookAt(eye[0], eye[1], eye[2],
		look[0], look[1], look[2],
		-down[0], -down[1], -down[2]);

}


#pragma endregion

#pragma region Visualization

void PPC::Visualize(WorldView *world, float vf, Vec3d colorBox, Vec3d colorPoint)
{
	world->Draw3DPoint(C, 7, colorPoint);

	Vec3d imageCorners[4];
	float _w = GetFocalLength() / vf;
	imageCorners[0] = UnProject(Vec3d(0.0f, 0.0f, _w));
	imageCorners[1] = UnProject(Vec3d(0.0f, (float)h, _w));
	imageCorners[2] = UnProject(Vec3d((float)w, (float)h, _w));
	imageCorners[3] = UnProject(Vec3d((float)w, 0.0f, _w));

	for (int i = 0; i < 4; i++)
	{
		world->Draw3DSegment(imageCorners[i], imageCorners[(i + 1) % 4], colorBox, colorBox);
	}
	world->Draw3DSegment(C, imageCorners[0], colorPoint, colorBox);
}

void PPC::Visualize(WorldView *world, WorldView *cameraWorld, float vf)
{
	float focal = GetFocalLength();
	FrameBuffer * cameraFb = cameraWorld->GetFB();
	for (int v = 0; v < cameraFb->h; v++)
	{
		for (int u = 0; u < cameraFb->w; u++)
		{
			if (cameraFb->GetZ(u, v) == 0.0f)
				continue;

			float _w = focal / vf;
			Vec3d pix3d = UnProject(Vec3d(0.5f + (float)u, 0.5f + (float)v, _w));
			Vec3d color = Vec3d::FromColor(cameraFb->Get(u, v));
			world->Draw3DPoint(pix3d, 1, color);
		}
	}
}

#pragma endregion
