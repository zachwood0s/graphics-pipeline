#include "stdafx.h"

#include <fstream>
#include <iostream>
#include <algorithm>

#include "TMesh.h"

using namespace std;


void TMesh::Allocate(int _vertsN, int _trisN) {

	vertsN = _vertsN;
	trisN = _trisN;
	verts = new Vec3d[vertsN];
	colors = new Vec3d[vertsN];
	normals = new Vec3d[vertsN];
	tris = new unsigned int[trisN * 3];
}

void TMesh::SetToCube(Vec3d cc, float sideLength, unsigned int color0, unsigned int color1) {

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

void TMesh::DrawCubeQuadFaces(FrameBuffer *fb, PPC *ppc, unsigned int color) {

	Vec3d c0;
	c0.SetFromColor(color);
	for (int si = 0; si < 4; si++) {
		fb->Draw3DSegment(verts[si], verts[(si + 1) % 4], ppc, c0, c0);
		fb->Draw3DSegment(verts[4+si], verts[4 + (si + 1) % 4], ppc, c0, c0);
		fb->Draw3DSegment(verts[si], verts[si + 4], ppc, c0, c0);
	}

}

void TMesh::DrawWireFrame(WorldView * view, unsigned int color) {

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

void TMesh::DrawInterpolated(WorldView * view, unsigned int color)
{
	for (int tri = 0; tri < trisN; tri++) {
		Vec3d V0 = verts[tris[3 * tri + 0]];
		Vec3d V1 = verts[tris[3 * tri + 1]];
		Vec3d V2 = verts[tris[3 * tri + 2]];
		Vec3d c0 = colors[tris[3 * tri + 0]];
		Vec3d c1 = colors[tris[3 * tri + 1]];
		Vec3d c2 = colors[tris[3 * tri + 2]];
		view->Draw3DTriangle(V0, V1, V2, c0, c1, c2);
	}
}

void TMesh::LoadBin(const char *fname) {

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

Vec3d TMesh::GetCenter() {

	Vec3d ret(0.0f, 0.0f, 0.0f);
	for (int vi = 0; vi < vertsN; vi++) {
		ret = ret + verts[vi];
	}
	ret = ret / (float)vertsN;
	return ret;

}

void TMesh::Translate(Vec3d tv) {

	for (int vi = 0; vi < vertsN; vi++) {
		verts[vi] = verts[vi] + tv;
	}

}

void TMesh::SetCenter(Vec3d center) {

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
