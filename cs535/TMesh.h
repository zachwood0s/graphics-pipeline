#pragma once

#include <vector>

#include "Vec3d.h"
#include "ppc.h"
#include "framebuffer.h"
#include "WorldView.h"
#include "Material.h"
#include "AABB.h"

struct InterpVal
{
	Vec3d edges;
	Vec3d colors;
	Vec3d normals;
	Vec3d texs;
	float zVal;
	float denom;
};

struct InterpCoefs
{
	InterpCoefs(Matrix3d _e, Matrix3d _c, Matrix3d _n, Matrix3d _t, Vec3d _z, Vec3d _d)
		: edges(_e), colors(_c), normals(_n), texs(_t), zVals(_z), denom(_d){}

	Matrix3d edges;
	Matrix3d colors;
	Matrix3d normals;
	Matrix3d texs;
	Vec3d zVals;
	Vec3d denom;

	InterpVal Start(Vec3d start)
	{
		return {
			edges * start,
			colors * start,
			normals * start,
			texs * start,
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
			prev.texs + texs.GetColumn(i),
			prev.zVal + zVals[i],
			prev.denom + denom[i]
		};
	}
};


struct TriangleMatrices
{
	Matrix3d projected;
	Matrix3d unprojected;
	Matrix3d colors;
	Matrix3d normals;
	Matrix3d textures;
};


class TMesh 
{
private:
	Material material;
	std::vector<Vec3d> projected;

	// This is private because the vertices need to be projected before entering this function
	void DrawModelSpaceInterpolated(Scene & scene, WorldView *world, Rect renderBounds, bool disableLighting);

public:
	int onFlag;
	std::vector<Vec3d> verts;
	std::vector<Vec3d> colors;
	std::vector<Vec3d> normals;
	std::vector<Vec3d> texs;
	std::vector<unsigned int> tris;
	std::vector<unsigned int> normalTris;
	std::vector<unsigned int> texTris;

	bool hasMaterial = false;

	TMesh();
	~TMesh();
	void SetToPlane(Vec3d cc, float w, float h);

	void DrawCubeQuadFaces(FrameBuffer *fb, PPC *ppc, unsigned int color);
	void DrawWireFrame(WorldView * world, unsigned int color);
	void DrawInterpolated(Scene &scene, WorldView * world);
	void DrawModelSpaceInterpolated(Scene & scene, WorldView *world, bool disableLighting);
	void DrawHW();

	void LoadBin(const char *fname);
	void LoadObj(const char *fname);

	Vec3d GetCenter() const;
	void SetCenter(Vec3d center);

	void SetMaterial(Material m);
	Material& GetMaterial();

	void Translate(Vec3d tv);
	void Rotate(Vec3d axisOrigin, Vec3d axisDir, float theta);
	AABB GetAABB() const;
	void ScaleTo(float size);


	void ProjectAll(WorldView* view, bool invertW = true);
	TriangleMatrices GetTriangleMatrices(int tri) const;
};