#pragma once

#include "Vec3d.h"
#include "ppc.h"
#include "framebuffer.h"
#include "WorldView.h"

struct InterpVal
{
	InterpVal(Vec3d e, Vec3d c, Vec3d n, float z, float d)
		: edges(e), colors(c), normals(n), zVal(z), denom(d)  {}

	Vec3d edges;
	Vec3d colors;
	Vec3d normals;
	float zVal;
	float denom;
};

struct InterpCoefs
{
	InterpCoefs(Matrix3d _e, Matrix3d _c, Matrix3d _n, Vec3d _z, Vec3d _d)
		: edges(_e), colors(_c), normals(_n), zVals(_z), denom(_d){}

	Matrix3d edges;
	Matrix3d colors;
	Matrix3d normals;
	Vec3d zVals;
	Vec3d denom;

	InterpVal Start(Vec3d start)
	{
		return {
			edges * start,
			colors * start,
			normals * start,
			zVals * start,
			denom * start
		};

	}

	InterpVal StepIdx(InterpVal prev, int i)
	{
		return {
			prev.edges + edges.GetColumn(i),
			prev.colors + colors.GetColumn(i),
			prev.normals + normals.GetColumn(i),
			prev.zVal + zVals[i],
			prev.denom + denom[i]
		};
	}
};

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
	~TMesh();
	void SetToCube(Vec3d cc, float sideLength, unsigned int color0, unsigned int color1);
	void Allocate(int _vertsN, int _trisN);
	void DrawCubeQuadFaces(FrameBuffer *fb, PPC *ppc, unsigned int color);
	void DrawWireFrame(WorldView * world, unsigned int color);
	void DrawInterpolated(WorldView * world, Vec3d C, Vec3d light, float kSpecular);
	void DrawModelSpaceInterpolated(WorldView *world);
	void LoadBin(const char *fname);
	Vec3d GetCenter() const;
	void SetCenter(Vec3d center);
	void Translate(Vec3d tv);
	void Rotate(Vec3d axisOrigin, Vec3d axisDir, float theta);
	AABB GetAABB() const;
	void ScaleTo(float size);

	Vec3d* ProjectAll(WorldView* view) const;
};