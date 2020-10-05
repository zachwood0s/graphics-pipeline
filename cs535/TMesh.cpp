#include "stdafx.h"

#include <fstream>
#include <iostream>
#include <algorithm>
#include <vector>
#include <omp.h>

#include "scene.h"
#include "TMesh.h"
#include "AABB.h"
#include "matrix3d.h"
#include "Material.h"

using namespace std;

const int RENDER_CUBE_COUNT = 4; // The number of cubes to render across

TMesh::~TMesh()
{
	delete[] verts;
	delete[] colors;
	delete[] normals;
	delete[] tris;
	delete[] texs;
	delete[] projected;
}

TMesh::TMesh()
	: verts(0), vertsN(0), tris(0), trisN(0), colors(0),
	normals(0), onFlag(1), material(Material::DEFAULT(Vec3d::ZEROS)), projected(nullptr)
{};


void TMesh::Allocate(int _vertsN, int _trisN) 
{
	vertsN = _vertsN;
	trisN = _trisN;
	verts = new Vec3d[vertsN];
	colors = new Vec3d[vertsN];
	normals = new Vec3d[normalsN];
	texs = new Vec3d[texsN];
	normalTris = new unsigned int[trisN * 3]{ 0 };
	texTris = new unsigned int[trisN * 3]{ 0 };
	tris = new unsigned int[trisN * 3]{ 0 };
}

#pragma region Preset Geometry

void TMesh::SetToPlane(Vec3d cc, float w, float h)
{
	vertsN = 4;
	trisN = 2;
	texsN = 4;
	normalsN = 1;
	Allocate(vertsN, trisN);

	int vi = 0;
	verts[vi] = cc + Vec3d(-w / 2.0f, +h / 2.0f, 0);
	vi++;
	verts[vi] = cc + Vec3d(-w / 2.0f, -h / 2.0f, 0);
	vi++;
	verts[vi] = cc + Vec3d(+w / 2.0f, -h / 2.0f, 0);
	vi++;
	verts[vi] = cc + Vec3d(+w / 2.0f, +h / 2.0f, 0);

	int ti = 0;
	texs[ti] = Vec3d(0.0f, 0.0f, 0.0f);
	ti++;
	texs[ti] = Vec3d(0.0f, 1.0f, 0.0f);
	ti++;
	texs[ti] = Vec3d(1.0f, 1.0f, 0.0f);
	ti++;
	texs[ti] = Vec3d(1.0f, 0.0f, 0.0f);

	int tri = 0;
	tris[3 * tri + 0] = 0;
	tris[3 * tri + 1] = 1;
	tris[3 * tri + 2] = 2;
	texTris[3 * tri + 0] = 0;
	texTris[3 * tri + 1] = 1;
	texTris[3 * tri + 2] = 2;
	tri++;
	tris[3 * tri + 0] = 2;
	tris[3 * tri + 1] = 3;
	tris[3 * tri + 2] = 0;
	texTris[3 * tri + 0] = 2;
	texTris[3 * tri + 1] = 3;
	texTris[3 * tri + 2] = 0;
	tri++;


	normals[0] = ((verts[0] - verts[1]) ^ (verts[0] - verts[2])).Normalized() * -1;
}


void TMesh::SetToCube(Vec3d cc, float sideLength, unsigned int color0, unsigned int color1) 
{
	vertsN = 8;
	texsN = 24;
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

#pragma endregion

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

void TMesh::DrawInterpolated(WorldView * view, Vec3d light)
{

	ProjectAll(view);

	FrameBuffer * fb = view->GetFB();

	for (int tri = 0; tri < trisN; tri++)
	{
		unsigned int vertexIdx[3] = { tris[3 * tri + 0], tris[3 * tri + 1], tris[3 * tri + 2] };
		unsigned int normalIdx[3] = { normalTris[3 * tri + 0], normalTris[3 * tri + 1], normalTris[3 * tri + 2] };

		// Create the point matrix and color matrix for this triangle
		Matrix3d pointM(projected[vertexIdx[0]], projected[vertexIdx[1]], projected[vertexIdx[2]]);
		Matrix3d colorM = Matrix3d::FromColumns(colors[vertexIdx[0]], colors[vertexIdx[1]], colors[vertexIdx[2]]);
		Matrix3d normalM = Matrix3d::FromColumns(normals[normalIdx[0]], normals[normalIdx[1]], normals[normalIdx[2]]);

		if (pointM[0][0] == FLT_MAX || pointM[1][0] == FLT_MAX || pointM[2][0] == FLT_MAX)
		{
			continue;
		}

		// Find the bounding box and skip the triangle if its empty
		Rect bounds = AABB::Clipped({ 0, fb->w, 0, fb->h }, { pointM[0], pointM[1], pointM[2] }).GetPixelRect();
		if (bounds.right < bounds.left || bounds.bottom < bounds.top)
		{
			continue;
		}
			
		// Compute the edge equations and interpolation
		Matrix3d edgeEqns = Matrix3d::EdgeEquations(pointM);
		Matrix3d screenSpaceInterp = Matrix3d::ScreenSpaceInterp(pointM);
		Vec3d zVals = pointM.GetColumn(2);

		// same as SSIM * colors[i] for each row
		Matrix3d colorCoefs = (screenSpaceInterp * colorM.Transposed()).Transposed();
		Matrix3d normalCoefs = (screenSpaceInterp * normalM.Transposed()).Transposed();
		Vec3d zCoefs = screenSpaceInterp * zVals;

		// Grab the min and max values for the for the interpolation parameters
		auto[minZ, maxZ] = zVals.Bounds();
		AABB colorBounds = AABB::FromMatrixColumns(colorM);

		// This is the same as t = a * left + b * top + c;
		Vec3d starting(bounds.left + .5f, bounds.top + .5f, 1);
		InterpCoefs coefs(edgeEqns, colorCoefs, normalCoefs, normalCoefs, zCoefs, Vec3d::ZEROS);
		InterpVal interpT = coefs.Start(starting);

		for (int currPixY = bounds.top; currPixY <= bounds.bottom; currPixY++, interpT = coefs.StepIdx(interpT, 1))
		{
			int exit_early = 0; //Used for when we exit the triangle, we know we can continue onto next line because triangles are convex;
			//Vec3d e = t, colorE = colorT;
			//float zE = zT;
			InterpVal interpE = interpT;

			for (int currPixX = bounds.left; currPixX <= bounds.right; currPixX++, interpE = coefs.StepIdx(interpE, 0))
			{
				if (interpE.edges[0] >= 0 && interpE.edges[1] >= 0 && interpE.edges[2] >= 0)
				{
					// Clamp the z value so that its valid for this triangle.
					float currZ = clamp(interpE.zVal, minZ, maxZ);
					if (fb->Farther(currPixX, currPixY, currZ))
					{
						continue;
					}

					// Get the material values
					Material currMat = hasMaterial ? material : Material::DEFAULT(interpE.colors);

					// Normal at current pixel
					Vec3d currPix = Vec3d(currPixX + .5f, currPixY + .5f, 1);
					Vec3d normalVector = interpE.normals.Normalized();

					// 3D surface point at current pixel
					Vec3d currP = view->GetPPC()->UnProject(Vec3d(currPix[0], currPix[1], currZ));

					// Light vector
					Vec3d lightVector = (light - currP).Normalized();
					Vec3d viewDir = (view->GetPPC()->C - currP).Normalized();
					Vec3d currColor = currMat.color.Light(lightVector, normalVector, viewDir, view->kAmbient, currMat);

					// Clamp each color to somewhere in their starting range.
					// This handles errors for colors outside the given color range.
					currColor.Clamp(Vec3d::ZEROS, Vec3d::ONES);

					fb->Set(currPixX, currPixY, currColor.GetColor(), tri);
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

/// <summary>
/// I wanted to try optimizing the triangle mesh rendering so I made it execute in parellel using OpenMP. This 
/// could probably be improved greatly but for now it works.
/// </summary>
void TMesh::DrawModelSpaceInterpolated(Scene &scene, WorldView *view)
{
	ProjectAll(view);
	int w = view->GetFB()->w;
	int h = view->GetFB()->h;
	int square = w / RENDER_CUBE_COUNT;
	int squaresW = std::ceil(w / (float) square);
	int squaresH = std::ceil(h / (float) square);

	#pragma omp parallel for
	for (int xy = 0; xy < squaresW*squaresH; ++xy) {
		int i = xy / squaresW;
		int j = xy % squaresW;
		//parallelize this code here
		int left = j * square ; int top = i * square;
		DrawModelSpaceInterpolated(scene, view, { left, left + square, top, top + square });
	}
}

void TMesh::DrawModelSpaceInterpolated(Scene &scene, WorldView *view, Rect renderBounds)
{
	FrameBuffer * fb = view->GetFB();

	for (int tri = 0; tri < trisN; tri++)
	{
		TriangleMatrices matrices = GetTriangleMatrices(tri);

		if (matrices.projected[0][0] == FLT_MAX || matrices.projected[1][0] == FLT_MAX || matrices.projected[2][0] == FLT_MAX)
		{
			continue;
		}

		// Find the bounding box and skip the triangle if its empty
		Rect bounds = AABB::Clipped(renderBounds, { matrices.projected[0], matrices.projected[1], matrices.projected[2] }).GetPixelRect();
		if (bounds.right < bounds.left || bounds.bottom < bounds.top)
		{
			continue;
		}

		// Compute the edge equations and interpolation
		Matrix3d edgeEqns = Matrix3d::EdgeEquations(matrices.projected);
		Matrix3d modelSpaceInterp = Matrix3d::ModelSpaceInterp(matrices.unprojected, view->GetPPC());
		Matrix3d modelSpaceInterpT = modelSpaceInterp.Transposed();
		Vec3d denomParam = modelSpaceInterpT * Vec3d::ONES;
		Vec3d zVals = matrices.projected.GetColumn(2);

		// Don't render back facing triangles, but flip the edge equations if necessary
		float area = edgeEqns.GetColumn(2) * Vec3d::ONES;
		if (area == 0 || fabsf(area) < 0.001f) continue; // Non-existant triangle
		if (area < 0)
		{
			edgeEqns = edgeEqns * -1;
			area = -area;
		}

		// same as MSIM^T * colors[i] for each row
		Matrix3d colorCoefs = matrices.colors * modelSpaceInterp;
		Matrix3d normalCoefs = matrices.normals * modelSpaceInterp;
		Matrix3d texCoefs = matrices.textures * modelSpaceInterp;
		Vec3d zCoefs = modelSpaceInterpT * zVals;

		// Grab the min and max values for the for the interpolation parameters
		auto[minZ, maxZ] = zVals.Bounds();
		AABB colorBounds = AABB::FromMatrixColumns(matrices.colors);

		// This is the same as t = a * left + b * top + c;
		Vec3d starting(bounds.left + .5f, bounds.top + .5f, 1);
		InterpCoefs coefs(edgeEqns, colorCoefs, normalCoefs, texCoefs, zCoefs, denomParam);
		InterpVal interpT = coefs.Start(starting);

		// Draw the triangle
		for (int currPixY = bounds.top; currPixY <= bounds.bottom; currPixY++, interpT = coefs.StepIdx(interpT, 1))
		{
			int exit_early = 0; //Used for when we exit the triangle, we know we can continue onto next line because triangles are convex;
			InterpVal interpE = interpT;

			for (int currPixX = bounds.left; currPixX <= bounds.right; currPixX++, interpE = coefs.StepIdx(interpE, 0))
			{
				if (interpE.edges[0] >= 0 && interpE.edges[1] >= 0 && interpE.edges[2] >= 0)
				{
					Vec3d currPix = Vec3d(currPixX + .5f, currPixY + .5f, 1);
					float denom = 1 / interpE.denom;

					// Clamp the z value so that its valid for this triangle.
					float currZ = clamp(interpE.zVal * denom, minZ, maxZ);
					if (fb->Farther(currPixX, currPixY, currZ, false))
					{
						continue;
					}

					// Get the material values
					Vec3d texDeltas(texCoefs.GetColumn(0).Length() * denom, texCoefs.GetColumn(1).Length() * denom, 0);
					Material currMat = hasMaterial ? material : Material::DEFAULT(interpE.colors * denom);
					auto[baseColor, alpha] = currMat.GetColor(scene, interpE.texs * denom, texDeltas);

					// Check to see if this pixel should even be rendered
					if (FP_ZERO == fpclassify(alpha))
					{
						continue; 
					}

					// If it should, update the z-buffer to reflect that
					fb->SetZ(currPixX, currPixY, currZ);

					// Normal at current pixel
					Vec3d normalVector = (interpE.normals * denom).Normalized();

					// 3D surface point at current pixel
					Vec3d currP = view->GetPPC()->UnProject(Vec3d(currPix[0], currPix[1], currZ));

					// Light vector
					Vec3d lightVector = (scene.light - currP).Normalized();
					Vec3d viewDir = (view->GetPPC()->C - currP).Normalized();
					Vec3d currColor = baseColor.Light(lightVector, normalVector, viewDir, view->kAmbient, currMat);

					// Clamp each color to somewhere in their starting range.
					// This handles errors for colors outside the given color range.
					currColor.Clamp(Vec3d::ZEROS, Vec3d::ONES);

					fb->Set(currPixX, currPixY, currColor.GetColor(), tri);
					exit_early++;
				}
				else if (exit_early != 0)
				{
					break; // Continue onto next line
				}

			}
		}

	}

}

#pragma region Loading from file

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
	normalsN = vertsN;
	texsN = 0;

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
	normalTris = tris;
	texTris = tris;
	texs = new Vec3d[vertsN]();
	ifs.read((char*)tris, trisN * 3 * sizeof(unsigned int)); // read tiangles

	ifs.close();

	std::cerr << "INFO: loaded " << vertsN << " verts, " << trisN << " tris from " << endl << "      " << fname << endl;
	std::cerr << "      xyz " << ((colors) ? "rgb " : "") << ((normals) ? "nxnynz " : "") << ((tcs) ? "tcstct " : "") << endl;

}

static int ReadPolyIndices(const char *line, unsigned int (&_vi)[3], unsigned int (&_ti)[3], unsigned int (&_ni)[3])
{

	/* Parse a face vertex specification from the given line. */

	if (sscanf_s(line, "%d/%d/%d %d/%d/%d %d/%d/%d", &_vi[0], &_ti[0], &_ni[0], 
													 &_vi[1], &_ti[1], &_ni[1], 
													 &_vi[2], &_ti[2], &_ni[2]) >= 3) return 1;
	if (sscanf_s(line, "%d/%d %d/%d %d/%d", &_vi[0], &_ti[0], 
											&_vi[1], &_ti[1], 
											&_vi[2], &_ti[2]) >= 2) return 1;
	if (sscanf_s(line, "%d//%d %d//%d %d//%d", &_vi[0], &_ni[0], 
											   &_vi[1], &_ni[1], 
											   &_vi[2], &_ni[2]) >= 2) return 1;
	if (sscanf_s(line, "%d %d %d", &_vi[0], &_vi[1], &_vi[2]) >= 1) return 1;

	return 0;
}

void TMesh::LoadObj(const char * fname)
{
	FILE * file;
	if (errno_t err = fopen_s(&file, fname, "r") != 0)
	{
		std::cerr << "INFO: cannot open file: " << fname << endl;
		return;
	}

	std::vector<unsigned int> vertexIndices, uvIndices, normalIndices;
	std::vector<Vec3d> tempVerts, tempUvs, tempNormals;


	while (1)
	{
		char lineHeader[128];

		int res = fscanf_s(file, "%s", lineHeader, (int) sizeof(lineHeader));
		if (res == EOF)
			break;

		if (strcmp(lineHeader, "v") == 0)
		{
			Vec3d vertex;
			fscanf_s(file, "%f %f %f\n", &vertex[0], &vertex[1], &vertex[2]);
			tempVerts.push_back(vertex);
		}
		else if (strcmp(lineHeader, "vt") == 0)
		{
			Vec3d vertex;
			vertex[2] = 0;
			fscanf_s(file, "%f %f\n", &vertex[0], &vertex[1]);
			vertex[1] = 1.0f - vertex[1]; // flip the y uv because we support texture coords with the y flipped
			tempUvs.push_back(vertex);
		}
		else if (strcmp(lineHeader, "vn") == 0)
		{
			Vec3d vertex;
			fscanf_s(file, "%f %f %f\n", &vertex[0], &vertex[1], &vertex[2]);
			tempNormals.push_back(vertex);
		}
		else if (strcmp(lineHeader, "f") == 0)
		{
			unsigned int vertexIndex[3] = { 1, 1, 1 }, uvIndex[3] = { 1, 1, 1 }, normalIndex[3] = { 1, 1, 1 };
			char buff[1024] = "";

			fscanf_s(file, "%1024[^\n]\n", buff, (int) sizeof(buff));
			if (!ReadPolyIndices(buff, vertexIndex, uvIndex, normalIndex))
			{
				std::cerr << "File cannot be read because of an unsupported face statement" << endl;
				fclose(file);
				file = nullptr;
				return;
			}
			
			vertexIndices.push_back(vertexIndex[0] - 1);
			vertexIndices.push_back(vertexIndex[1] - 1);
			vertexIndices.push_back(vertexIndex[2] - 1);
			uvIndices.push_back(uvIndex[0] - 1);
			uvIndices.push_back(uvIndex[1] - 1);
			uvIndices.push_back(uvIndex[2] - 1);
			normalIndices.push_back(normalIndex[0] - 1);
			normalIndices.push_back(normalIndex[1] - 1);
			normalIndices.push_back(normalIndex[2] - 1);
		}
	}

	vertsN = (int) tempVerts.size();
	verts = new Vec3d[vertsN];
	normalsN = std::max(1, (int) tempNormals.size());
	normals = new Vec3d[normalsN];
	colors = new Vec3d[vertsN]; // Not used for this set
	texsN = std::max(1, (int)tempUvs.size());
	texs = new Vec3d[std::max(1, (int) tempUvs.size())];
	trisN = (int) vertexIndices.size() / 3;
	tris = new unsigned int[trisN * 3]{};
	normalTris = new unsigned int[trisN * 3]{};
	texTris = new unsigned int[trisN * 3]{};

	std::fill_n(tris, trisN * 3, 0);
	std::fill_n(normalTris, trisN * 3, 0);
	std::fill_n(texTris, trisN * 3, 0);

	for (int i = 0; i < tempVerts.size(); i++)
	{
		verts[i] = tempVerts[i];
	}

	for (int i = 0; i < tempNormals.size(); i++)
	{
		normals[i] = tempNormals[i];
	}

	for (int i = 0; i < tempUvs.size(); i++)
	{
		texs[i] = tempUvs[i];
	}

	for (int i = 0; i < vertexIndices.size(); i++)
	{
		tris[i] = vertexIndices[i];
	}

	for (int i = 0; i < uvIndices.size(); i++)
	{
		texTris[i] = uvIndices[i];
	}

	for (int i = 0; i < normalIndices.size(); i++)
	{
		normalTris[i] = normalIndices[i];
	}

	fclose(file);
	file = nullptr;

	cerr << "INFO: loaded " << vertsN << " verts, " << trisN << " tris from " << endl << "      " << fname << endl;
	cerr << "      xyz " << ((colors) ? "rgb " : "") << ((normals) ? "nxnynz " : "") << ((texs) ? "tcstct " : "") << endl;
}

#pragma endregion

Vec3d TMesh::GetCenter() const
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

void TMesh::SetMaterial(Material m)
{
	material = m;
	hasMaterial = true;
}

Material& TMesh::GetMaterial()
{
	return material;
}

void TMesh::Rotate(Vec3d axisOrigin, Vec3d axisDir, float theta)
{
	for (int vi = 0; vi < vertsN; vi++)
	{
		verts[vi] = verts[vi].Rotate(axisOrigin, axisDir, theta);
	}
	for (int ni = 0; ni < normalsN; ni++)
	{
		normals[ni] = normals[ni].Rotate(axisOrigin, axisDir, theta);
	}
}

AABB TMesh::GetAABB() const
{
	if (vertsN == 0)
	{
		return AABB(Vec3d::ZEROS);
	}

	AABB aabb(verts[0]);

	for (int vi = 1; vi < vertsN; vi++)
	{
		aabb.AddPoint(verts[vi]);
	}
	return aabb;
}

void TMesh::ScaleTo(float size)
{	
	AABB aabb = GetAABB();

	Vec3d currCenter = GetCenter();

	// Move centroid to origin
	SetCenter(Vec3d::ZEROS);

	// Get the length of the current diagonal
	float currSize = (aabb.Max() - aabb.Min()).Length();
	float scaleFactor = size / currSize;

	Vec3d center = GetCenter();
	for (int vi = 0; vi < vertsN; vi++)
	{
		verts[vi] = verts[vi] * scaleFactor;
	}

	// Move back to original center
	SetCenter(currCenter);
}

void TMesh::ProjectAll(WorldView* view) 
{
	if (projected == nullptr)
	{
		projected = new Vec3d[vertsN];
	}

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
}

TriangleMatrices TMesh::GetTriangleMatrices(int tri) const
{
	unsigned int vertexIdx[3] = { tris[3 * tri + 0], tris[3 * tri + 1], tris[3 * tri + 2] };
	unsigned int normalIdx[3] = { normalTris[3 * tri + 0], normalTris[3 * tri + 1], normalTris[3 * tri + 2] };
	unsigned int texIdx[3] = { texTris[3 * tri + 0], texTris[3 * tri + 1], texTris[3 * tri + 2] };

	// Create the point matrix and color matrix for this triangle
	Matrix3d pointM(projected[vertexIdx[0]], projected[vertexIdx[1]], projected[vertexIdx[2]]);
	Matrix3d unProjected(verts[vertexIdx[0]], verts[vertexIdx[1]], verts[vertexIdx[2]]);
	Matrix3d colorM = Matrix3d::FromColumns(colors[vertexIdx[0]], colors[vertexIdx[1]], colors[vertexIdx[2]]);
	Matrix3d normalM = Matrix3d::FromColumns(normals[normalIdx[0]], normals[normalIdx[1]], normals[normalIdx[2]]);
	Matrix3d texM = Matrix3d::FromColumns(texs[texIdx[0]], texs[texIdx[1]], texs[texIdx[2]]);
	texM[2] = Vec3d::ZEROS;
	return { pointM, unProjected, colorM, normalM, texM };
}
