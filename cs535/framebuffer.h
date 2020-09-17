#pragma once

#include <FL/Fl.H>
#include <FL/Fl_Gl_Window.H>
#include <GL/glut.h>

#include "vec3d.h"
#include "ppc.h"

class FrameBuffer : public Fl_Gl_Window {
public:
	unsigned int *pix; // pixel array
	float *zb;
	int w, h;
	FrameBuffer(int u0, int v0, int _w, int _h, unsigned int _id);
	void draw();
	void KeyboardHandle();
	int handle(int guievent);
	void SetBGR(unsigned int bgr);
	void Set(int u, int v, unsigned int color);
	void DrawRect(int u, int v, int w, int h, unsigned int color);
	void DrawCircle(int u, int v, int r, unsigned int color);
	void Draw2DTriangle(Vec3d p1, Vec3d p2, Vec3d p3, Vec3d c1, Vec3d c2, Vec3d c3);
	void Draw3DTriangle(Vec3d P1, Vec3d P2, Vec3d P3, PPC *ppc, Vec3d c1, Vec3d c2, Vec3d c3);
	void Draw3DSegment(Vec3d P0, Vec3d P1, PPC *ppc, Vec3d c0, Vec3d c1);
	void Draw2DSegment(Vec3d p0, Vec3d c0, Vec3d p1, Vec3d c1);
	int Farther(int u, int v, float currz);
	void LoadTiff(char* fname);
	void SaveAsTiff(const char *fname);
	void ClearZB();
};