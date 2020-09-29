#include "stdafx.h"

#include <fstream>
#include <iostream>
#include <algorithm>

#include "TMesh.h"
#include "AABB.h"
#include "matrix3d.h"

using namespace std;

TMesh::~TMesh()
{
	delete verts;
	delete colors;
	delete normals;
	delete tris;
}


void TMesh::Allocate(int _vertsN, int _trisN) 
{
	vertsN = _vertsN;
	trisN = _trisN;
	verts = new Vec3d[vertsN];
	colors = new Vec3d[vertsN];
	normals = new Vec3d[vertsN];
	tris = new unsigned int[trisN * 3];
}


void TMesh::SetToCube(Vec3d cc, float sideLength, unsigned int color0, unsigned int color1) 
{
	vertsN = 8;
	trisN = 6 * 2;
	Allocate(vertsN, trisN);

	for (int vi = 0; vi < 4; vi++) {
		colors[vi].SetFromColor(color0);
		colors[vi+4].SetFromColor(color1);
	}

	int vi = 0;
	verts[vi] = cc + Vec3d(-sideLength / 2.0f, +sideLength / 2.0f, +sideLength / 2.0f);
	vi++;
	verts[vi] = cc + Vec3d(-sideLength / 2.0f, -sideLength / 2.0f, +sideLength / 2.0f);
	vi++;
	verts[vi] = cc + Vec3d(+sideLength / 2.0f, -sideLength / 2.0f, +sideLength / 2.0f);
	vi++;
	verts[vi] = cc + Vec3d(+sideLength / 2.0f, +sideLength / 2.0f, +sideLength / 2.0f);
	vi++;

	verts[vi] = verts[vi - 4] + Vec3d(0.0f, 0.0f, -sideLength);
	vi++;
	verts[vi] = verts[vi - 4] + Vec3d(0.0f, 0.0f, -sideLength);
	vi++;
	verts[vi] = verts[vi - 4] + Vec3d(0.0f, 0.0f, -sideLength);
	vi++;
	verts[vi] = verts[vi - 4] + Vec3d(0.0f, 0.0f, -sideLength);
	vi++;

	int tri = 0;
	tris[3 * tri + 0] = 0;
	tris[3 * tri + 1] = 1;
	tris[3 * tri + 2] = 2;
	tri++;
	tris[3 * tri + 0] = 2;
	tris[3 * tri + 1] = 3;
	tris[3 * tri + 2] = 0;
	tri++;
	tris[3 * tri + 0] = 3;
	tris[3 * tri + 1] = 2;
	tris[3 * tri + 2] = 6;
	tri++;
	tris[3 * tri + 0] = 6;
	tris[3 * tri + 1] = 7;
	tris[3 * tri + 2] = 3;
	tri++;
	tris[3 * tri + 0] = 7;
	tris[3 * tri + 1] = 6;
	tris[3 * tri + 2] = 5;
	tri++;
	tris[3 * tri + 0] = 5;
	tris[3 * tri + 1] = 4;
	tris[3 * tri + 2] = 7;
	tri++;
	tris[3 * tri + 0] = 4;
	tris[3 * tri + 1] = 5;
	tris[3 * tri + 2] = 1;
	tri++;
	tris[3 * tri + 0] = 1;
	tris[3 * tri + 1] = 0;
	tris[3 * tri + 2] = 4;
	tri++;
	tris[3 * tri + 0] = 4;
	tris[3 * tri + 1] = 0;
	tris[3 * tri + 2] = 3;
	tri++;
	tris[3 * tri + 0] = 3;
	tris[3 * tri + 1] = 7;
	tris[3 * tri + 2] = 4;
	tri++;
	tris[3 * tri + 0] = 1;
	tris[3 * tri + 1] = 5;
	tris[3 * tri + 2] = 6;
	tri++;
	tris[3 * tri + 0] = 6;
	tris[3 * tri + 1] = 2;
	tris[3 * tri + 2] = 1;
	tri++;

}

void TMesh::DrawCubeQuadFaces(FrameBuffer *fb, PPC *ppc, unsigned int color) 
{

	Vec3d c0 = Vec3d::FromColor(color);
	for (int si = 0; si < 4; si++) {
		fb->Draw3DSegment(verts[si], verts[(si + 1) % 4], ppc, c0, c0);
		fb->Draw3DSegment(verts[4+si], verts[4 + (si + 1) % 4], ppc, c0, c0);
		fb->Draw3DSegment(verts[si], verts[si + 4], ppc, c0, c0);
	}

}

void TMesh::DrawWireFrame(WorldView * view, unsigned int color) 
{
	for (int tri = 0; tri < trisN; tri++) {
		Vec3d V0 = verts[tris[3 * tri + 0]];
		Vec3d V1 = verts[tris[3 * tri + 1]];
		Vec3d V2 = verts[tris[3 * tri + 2]];
		Vec3d c0 = colors[tris[3 * tri + 0]];
		Vec3d c1 = colors[tris[3 * tri + 1]];
		Vec3d c2 = colors[tris[3 * tri + 2]];
		view->Draw3DSegment(V0, V1, c0, c1);
		view->Draw3DSegment(V1, V2, c1, c2);
		view->Draw3DSegment(V2, V0, c2, c0);
	}

}

void TMesh::DrawInterpolated(WorldView * view)
{

	Vec3d *projected = ProjectAll(view);
	FrameBuffer * fb = view->GetFB();

	for (int tri = 0; tri < trisN; tri++)
	{
		unsigned int vertexIdx[3] = { tris[3 * tri + 0], tris[3 * tri + 1], tris[3 * tri + 2] };

		// Create the point matrix and color matrix for this triangle
		Matrix3d pointM(projected[vertexIdx[0]], projected[vertexIdx[1]], projected[vertexIdx[2]]);
		Matrix3d colorM = Matrix3d::FromColumns(colors[vertexIdx[0]], colors[vertexIdx[1]], colors[vertexIdx[2]]);

		if (pointM[0][0] == FLT_MAX || pointM[1][0] == FLT_MAX || pointM[2][0] == FLT_MAX)
		{
			continue;
		}

		// Find the bounding box
		Rect bounds = AABB::Clipped(fb->w, fb->h, { pointM[0], pointM[1], pointM[2] }).GetPixelRect();
			
		// Compute the edge equations and interpolation
		Matrix3d edgeEqns = Matrix3d::EdgeEquations(pointM);
		Matrix3d screenSpaceInterp = Matrix3d::ScreenSpaceInterp(pointM);
		Vec3d zVals = pointM.GetColumn(2);

		// same as SSIM * colors[i] for each row
		Matrix3d colorCoefs = (screenSpaceInterp * colorM.Transposed()).Transposed();
		Vec3d zCoefs = screenSpaceInterp * zVals;

		// Grab the min and max values for the for the interpolation parameters
		auto[minZ, maxZ] = zVals.Bounds();
		AABB colorBounds = std::make_from_tuple<AABB>(colorM.Columns());

		// Grab the coefficients individuall so that interp can be done incrementally
		auto[a, b, c] = edgeEqns.Columns();

		// This is the same as t = a * left + b * top + c;
		Vec3d t = edgeEqns * Vec3d(bounds.left + .5f, bounds.top + .5f, 1);

		for (int currPixY = bounds.top; currPixY <= bounds.bottom; currPixY++, t = t + b)
		{
			int exit_early = 0; //Used for when we exit the triangle, we know we can continue onto next line because triangles are convex;
			Vec3d e = t;

			for (int currPixX = bounds.left; currPixX <= bounds.right; currPixX++, e = e + a)
			{
				if (e[0] >= 0 && e[1] >= 0 && e[2] >= 0)
				{
					Vec3d p = Vec3d((float) currPixX, (float) currPixY, 1);

					// Clamp the z value so that its valid for this triangle.
					float currZ = clamp(zCoefs * p, minZ, maxZ);
					if (fb->Farther((int) p[0], (int) p[1], currZ))
					{
						continue;
					}
					Vec3d color = colorCoefs * p;

					// Clamp each color to somewhere in their starting range.
					// This handles errors for colors outside the given color range.
					color.Clamp(colorBounds);

					fb->Set(currPixX, currPixY, color.GetColor(), tri);
					exit_early++;
				}
				else if (exit_early != 0)
				{
					break; // Continue onto next line
				}

			}
		}

	}

	delete[] projected;
}

void TMesh::DrawModelSpaceInterpolated(WorldView *view)
{
	Vec3d *projected = ProjectAll(view);
	FrameBuffer * fb = view->GetFB();

	for (int tri = 0; tri < trisN; tri++)
	{
		unsigned int vertexIdx[3] = { tris[3 * tri + 0], tris[3 * tri + 1], tris[3 * tri + 2] };

		// Create the point matrix and color matrix for this triangle
		Matrix3d pointM(projected[vertexIdx[0]], projected[vertexIdx[1]], projected[vertexIdx[2]]);
		Matrix3d colorM = Matrix3d::FromColumns(colors[vertexIdx[0]], colors[vertexIdx[1]], colors[vertexIdx[2]]);

		if (pointM[0][0] == FLT_MAX || pointM[1][0] == FLT_MAX || pointM[2][0] == FLT_MAX)
		{
			continue;
		}

		// Find the bounding box
		Rect bounds = AABB::Clipped(fb->w, fb->h, { pointM[0], pointM[1], pointM[2] }).GetPixelRect();
			
		// Compute the edge equations and interpolation
		Matrix3d edgeEqns = Matrix3d::EdgeEquations(pointM);
		Matrix3d modelSpaceInterp = Matrix3d::ModelSpaceInterp(pointM, view->GetPPC());
		Vec3d denomParam = modelSpaceInterp * Vec3d::ONES;
		Vec3d zVals = pointM.GetColumn(2);

		// same as MSIM * colors[i] for each row
		Matrix3d colorCoefs = (modelSpaceInterp * colorM.Transposed()).Transposed();
		Vec3d zCoefs = modelSpaceInterp * zVals;

		// Grab the min and max values for the for the interpolation parameters
		auto[minZ, maxZ] = zVals.Bounds();
		AABB colorBounds = std::make_from_tuple<AABB>(colorM.Columns());

		// Grab the coefficients individual so that interp can be done incrementally
		auto[a, b, c] = edgeEqns.Columns();

		// This is the same as t = a * left + b * top + c;
		Vec3d t = edgeEqns * Vec3d(bounds.left + .5f, bounds.top + .5f, 1);

		// Draw the triangle
		for (int currPixY = bounds.top; currPixY <= bounds.bottom; currPixY++, t = t + b)
		{
			int exit_early = 0; //Used for when we exit the triangle, we know we can continue onto next line because triangles are convex;
			Vec3d e = t;

			for (int currPixX = bounds.left; currPixX <= bounds.right; currPixX++, e = e + a)
			{
				if (e[0] >= 0 && e[1] >= 0 && e[2] >= 0)
				{
					Vec3d p = Vec3d((float) currPixX, (float) currPixY, 1);
					float denom = 1 / (denomParam * p);

					// Clamp the z value so that its valid for this triangle.
					float currZ = zCoefs * p * denom;//clamp(zCoefs * p * denom, minZ, maxZ);
					if (fb->Farther((int) p[0], (int) p[1], currZ))
					{
						continue;
					}

					Vec3d color = colorCoefs * p * denom;

					// Clamp each color to somewhere in their starting range.
					// This handles errors for colors outside the given color range.
					//color.Clamp(colorBounds.Min(), colorBounds.Max());

					fb->Set(currPixX, currPixY, color.GetColor(), tri);
					exit_early++;
				}
				else if (exit_early != 0)
				{
					break; // Continue onto next line
				}

			}
		}

	}

	delete[] projected;
}

void TMesh::LoadBin(const char *fname) 
{
	ifstream ifs(fname, ios::binary);
	if (ifs.fail()) {
		cerr << "INFO: cannot open file: " << fname << endl;
		return;
	}

	ifs.read((char*)&vertsN, sizeof(int));
	char yn;
	ifs.read(&yn, 1); // always xyz
	if (yn != 'y') {
		cerr << "INTERNAL ERROR: there should always be vertex xyz data" << endl;
		return;
	}
	if (verts)
		delete verts;
	verts = new Vec3d[vertsN];

	ifs.read(&yn, 1); // cols 3 floats
	if (colors)
		delete colors;
	colors = 0;
	if (yn == 'y') {
		colors = new Vec3d[vertsN];
	}

	ifs.read(&yn, 1); // normals 3 floats
	if (normals)
		delete normals;
	normals = 0;
	if (yn == 'y') {
		normals = new Vec3d[vertsN];
	}

	ifs.read(&yn, 1); // texture coordinates 2 floats
	float *tcs = 0; // don't have texture coordinates for now
	if (tcs)
		delete tcs;
	tcs = 0;
	if (yn == 'y') {
		tcs = new float[vertsN * 2];
	}

	ifs.read((char*)verts, vertsN * 3 * sizeof(float)); // load verts

	if (colors) {
		ifs.read((char*)colors, vertsN * 3 * sizeof(float)); // load cols
	}

	if (normals)
		ifs.read((char*)normals, vertsN * 3 * sizeof(float)); // load normals

	if (tcs)
		ifs.read((char*)tcs, vertsN * 2 * sizeof(float)); // load texture coordinates

	ifs.read((char*)&trisN, sizeof(int));
	if (tris)
		delete tris;
	tris = new unsigned int[trisN * 3];
	ifs.read((char*)tris, trisN * 3 * sizeof(unsigned int)); // read tiangles

	ifs.close();

	cerr << "INFO: loaded " << vertsN << " verts, " << trisN << " tris from " << endl << "      " << fname << endl;
	cerr << "      xyz " << ((colors) ? "rgb " : "") << ((normals) ? "nxnynz " : "") << ((tcs) ? "tcstct " : "") << endl;

}

Vec3d TMesh::GetCenter() 
{
	Vec3d ret(0.0f, 0.0f, 0.0f);
	for (int vi = 0; vi < vertsN; vi++) {
		ret = ret + verts[vi];
	}
	ret = ret / (float)vertsN;
	return ret;
}

void TMesh::Translate(Vec3d tv) 
{
	for (int vi = 0; vi < vertsN; vi++) {
		verts[vi] = verts[vi] + tv;
	}
}

void TMesh::SetCenter(Vec3d center) 
{
	Vec3d currCenter = GetCenter();
	Translate(center - currCenter);
}

void TMesh::Rotate(Vec3d axisOrigin, Vec3d axisDir, float theta)
{
	for (int vi = 0; vi < vertsN; vi++)
	{
		verts[vi] = verts[vi].Rotate(axisOrigin, axisDir, theta);
	}
}

void TMesh::GetAABB(Vec3d &smallest, Vec3d &largest)
{
	if (vertsN == 0)
	{
		return;
	}

	smallest = verts[0];
	largest = verts[0];
	for (int vi = 1; vi < vertsN; vi++)
	{
		smallest[0] = min(smallest[0], verts[vi][0]);
		smallest[1] = min(smallest[1], verts[vi][1]);
		smallest[2] = min(smallest[2], verts[vi][2]);
		largest[0] = max(largest[0], verts[vi][0]);
		largest[1] = max(largest[1], verts[vi][1]);
		largest[2] = max(largest[2], verts[vi][2]);
	}
}

void TMesh::ScaleTo(float size)
{	
	Vec3d smallest, largest;
	GetAABB(smallest, largest);

	Vec3d currCenter = GetCenter();

	// Move centroid to origin
	SetCenter(Vec3d(0, 0, 0));

	// Get the length of the current diagonal
	float currSize = (largest - smallest).Length();
	float scaleFactor = size / currSize;

	Vec3d center = GetCenter();
	for (int vi = 0; vi < vertsN; vi++)
	{
		verts[vi] = verts[vi] * scaleFactor;
	}

	// Move back to original center
	SetCenter(currCenter);
}

Vec3d* TMesh::ProjectAll(WorldView* view) const
{
	Vec3d *projected = new Vec3d[vertsN];

	// Project all of the verticies
	for (int i = 0; i < vertsN; i++)
	{
		if (!view->GetPPC()->Project(verts[i], projected[i]))
		{
			projected[i] = Vec3d(FLT_MAX, FLT_MAX, FLT_MAX);
		}
		else
		{
			// Clamp the projected points onto a 64x64 sub-grid 
			// This helps with numerical precision issues
			projected[i].Clamp(64);
		}
	}

	return projected;
}
