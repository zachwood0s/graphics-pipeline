#pragma once

#include "Vec3d.h"
#include "ppc.h"
#include "framebuffer.h"
#include "WorldView.h"

class TMesh {
public:
	int onFlag;
	Vec3d *verts;
	Vec3d *colors;
	Vec3d *normals;
	int vertsN;
	unsigned int *tris;
	int trisN;
	TMesh() : verts(0), vertsN(0), tris(0), trisN(0), colors(0), normals(0), onFlag(1) {};
	void SetToCube(Vec3d cc, float sideLength, unsigned int color0, unsigned int color1);
	void Allocate(int _vertsN, int _trisN);
	void DrawCubeQuadFaces(FrameBuffer *fb, PPC *ppc, unsigned int color);
	void DrawWireFrame(WorldView * world, unsigned int color);
	void DrawInterpolated(WorldView * world, unsigned int color);
	void LoadBin(const char *fname);
	Vec3d GetCenter();
	void SetCenter(Vec3d center);
	void Translate(Vec3d tv);
	void Rotate(Vec3d axisOrigin, Vec3d axisDir, float theta);
	void GetAABB(Vec3d &smallest, Vec3d &largest);
	void ScaleTo(float size);
};