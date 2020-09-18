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
	int *ids; // ids of the triangle that set the pixel

	int w, h;
	FrameBuffer(int u0, int v0, int _w, int _h, const char * l);
	void draw();
	void KeyboardHandle();
	int handle(int guievent);

	void SetBGR(unsigned int bgr);
	void Set(int u, int v, unsigned int color);
	void Set(int u, int v, unsigned int color, int id);

	void Draw2DTriangle(Vec3d p1, Vec3d p2, Vec3d p3, Vec3d c1, Vec3d c2, Vec3d c3, int id = -1);
	void Draw3DTriangle(Vec3d P1, Vec3d P2, Vec3d P3, PPC *ppc, Vec3d c1, Vec3d c2, Vec3d c3, int id = -1);
	void Draw3DSegment(Vec3d P0, Vec3d P1, PPC *ppc, Vec3d c0, Vec3d c1);
	void Draw2DSegment(Vec3d p0, Vec3d p1, Vec3d c0, Vec3d c1);
	void Draw2DSquarePoint(Vec3d p, int psize, Vec3d color);
	void Draw3DSquarePoint(Vec3d p, PPC *ppc, int psize, Vec3d color);
	void Draw2DPoint(Vec3d p, int radius, Vec3d color);
	void Draw3DPoint(Vec3d p, PPC *ppc, int radius, Vec3d color);

	int Farther(int u, int v, float currz);
	float GetZ(int u, int v);
	unsigned int Get(int u, int v);
	int GetId(int u, int v);

	void LoadTiff(char* fname);
	void SaveAsTiff(const char *fname);
	void ClearZB();
};