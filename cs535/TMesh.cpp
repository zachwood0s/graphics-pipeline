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
#include "shaders.h"

using namespace std;

const int RENDER_SQUARE_COUNT = 4; // The number of cubes to render across

TMesh::~TMesh()
{
}

TMesh::TMesh()
	: onFlag(1), material(Material::DEFAULT(Vec3d::ZEROS))
{};



#pragma region Preset Geometry

void TMesh::SetToPlane(Vec3d cc, float w, float h)
{

	verts.push_back(cc + Vec3d(-w / 2.0f, +h / 2.0f, 0));
	verts.push_back(cc + Vec3d(-w / 2.0f, -h / 2.0f, 0));
	verts.push_back(cc + Vec3d(+w / 2.0f, -h / 2.0f, 0));
	verts.push_back(cc + Vec3d(+w / 2.0f, +h / 2.0f, 0));

	texs.push_back(Vec3d(0.0f, 0.0f, 0.0f));
	texs.push_back(Vec3d(0.0f, 1.0f, 0.0f));
	texs.push_back(Vec3d(1.0f, 1.0f, 0.0f));
	texs.push_back(Vec3d(1.0f, 0.0f, 0.0f));

	tris.push_back(0);
	tris.push_back(1);
	tris.push_back(2);
	texTris.push_back(0);
	texTris.push_back(1);
	texTris.push_back(2);

	tris.push_back(2);
	tris.push_back(3);
	tris.push_back(0);
	texTris.push_back(2);
	texTris.push_back(3);
	texTris.push_back(0);


	normals.push_back(((verts[0] - verts[1]) ^ (verts[0] - verts[2])).Normalized() * -1);
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
	for (int tri = 0; tri < tris.size() / 3; tri++) {
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

void TMesh::DrawInterpolated(Scene &scene, WorldView * view)
{

	ProjectAll(view);

	FrameBuffer * fb = view->GetFB();

	for (int tri = 0; tri < tris.size() / 3; tri++)
	{
		TriangleMatrices matrices = GetTriangleMatrices(tri);

		if(!matrices.projected.IsValid())
		//if (pointM[0][0] == FLT_MAX || pointM[1][0] == FLT_MAX || pointM[2][0] == FLT_MAX)
		{
			continue;
		}

		// Find the bounding box and skip the triangle if its empty
		//Rect bounds = AABB::Clipped({ 0, fb->w, 0, fb->h }, { pointM[0], pointM[1], pointM[2] }).GetPixelRect();
		Rect bounds = AABB::Clipped({ 0, fb->w, 0, fb->h }, { matrices.projected[0], matrices.projected[1], matrices.projected[2] }).GetPixelRect();
		if (bounds.right < bounds.left || bounds.bottom < bounds.top)
		{
			continue;
		}
			
		// Compute the edge equations and interpolation
		Matrix3d edgeEqns = Matrix3d::EdgeEquations(matrices.projected);
		Matrix3d screenSpaceInterp = Matrix3d::ScreenSpaceInterp(matrices.projected);
		Vec3d zVals = matrices.projected.GetColumn(2);

		// same as SSIM * colors[i] for each row
		Matrix3d colorCoefs = (screenSpaceInterp * matrices.projected.Transposed()).Transposed();
		Matrix3d normalCoefs = (screenSpaceInterp * matrices.normals.Transposed()).Transposed();
		Matrix3d texCoefs = (screenSpaceInterp * matrices.textures.Transposed()).Transposed();
		Vec3d zCoefs = screenSpaceInterp * zVals;

		// Grab the min and max values for the for the interpolation parameters
		auto[minZ, maxZ] = zVals.Bounds();
		AABB colorBounds = AABB::FromMatrixColumns(matrices.colors);

		// This is the same as t = a * left + b * top + c;
		Vec3d starting(bounds.left + .5f, bounds.top + .5f, 1);
		InterpCoefs coefs(edgeEqns, colorCoefs, normalCoefs, texCoefs, zCoefs, Vec3d::ZEROS);
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
					Vec3d texDeltas(texCoefs.GetColumn(0).Length(), texCoefs.GetColumn(1).Length(), 0);
					Material currMat = hasMaterial ? material : Material::DEFAULT(interpE.colors);
					auto[baseColor, alpha] = currMat.GetColor(scene, interpE.texs, texDeltas);

					fb->SetZ(currPixX, currPixY, currZ);
					Vec3d currColor = baseColor;
					Vec3d currP = view->GetPPC()->UnProject(Vec3d(currPixX, currPixY, currZ));

					for (auto shader : view->shaders)
					{
						currColor = shader({ scene, *view, currMat, interpE, currColor, currP, Vec3d(currPixX, currPixY, currZ) });
					}

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

/// <summary>
/// I wanted to try optimizing the triangle mesh rendering so I made it execute in parellel using OpenMP. This 
/// could probably be improved greatly but for now it works.
/// </summary>
void TMesh::DrawModelSpaceInterpolated(Scene &scene, WorldView *view, bool disableLighting)
{
	ProjectAll(view, false);
	int w = view->GetFB()->w;
	int h = view->GetFB()->h;
	int square = w / RENDER_SQUARE_COUNT;
	int squaresW = std::ceil(w / (float) square);
	int squaresH = std::ceil(h / (float) square);

	#pragma omp parallel for
	for (int xy = 0; xy < squaresW*squaresH; ++xy) {
		int i = xy / squaresW;
		int j = xy % squaresW;
		//parallelize this code here
		int left = j * square ; int top = i * square;
		DrawModelSpaceInterpolated(scene, view, { left, left + square, top, top + square }, disableLighting);
	}
}

void TMesh::DrawModelSpaceInterpolated(Scene &scene, WorldView *view, Rect renderBounds, bool disableLighting)
{
	FrameBuffer * fb = view->GetFB();

	for (int tri = 0; tri < tris.size() / 3; tri++)
	{
		TriangleMatrices matrices = GetTriangleMatrices(tri);

		if(!matrices.projected.IsValid())
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

		// same as MSIM^T * colors[i] for each row
		Matrix3d colorCoefs = matrices.colors * modelSpaceInterp;
		Matrix3d normalCoefs = matrices.normals * modelSpaceInterp;
		Matrix3d texCoefs = matrices.textures * modelSpaceInterp;
		Vec3d zCoefs = modelSpaceInterpT * zVals;

		// Grab the min and max values for the for the interpolation parameters
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
					float denom = 1 / interpE.denom;
					Vec3d texDeltas(texCoefs.GetColumn(0).Length() * denom, texCoefs.GetColumn(1).Length() * denom, 0);
					float currZ = 1 / (interpE.zVal * denom);

					// Compute the current interp by multiplying by the denominator
					InterpVal interp = { interpE.edges, interpE.colors * denom, interpE.normals * denom, texDeltas, currZ, denom };

					// Clamp the z value so that its valid for this triangle.
					if (fb->Farther(currPixX, currPixY, currZ, false))
					{
						continue;
					}

					// Get the material values
					Material currMat = hasMaterial ? material : Material::DEFAULT(interp.colors);
					auto[baseColor, alpha] = currMat.GetColor(scene, interpE.texs * denom, interp.texs);

					// Check to see if this pixel should even be rendered
					if (FP_ZERO == fpclassify(alpha))
					{
						continue; 
					}

					// If it should, update the z-buffer to reflect that:w
					fb->SetZ(currPixX, currPixY, currZ);
					Vec3d currColor = baseColor;
					Vec3d currP = view->GetPPC()->UnProject(Vec3d(currPixX, currPixY, currZ));

					// Call each shader in order, passing in the updated color into each. 
					for (auto shader : view->shaders)
					{
						currColor = shader({ scene, *view, currMat, interp, currColor, currP, Vec3d(currPixX, currPixY, currZ) });
					}

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
	int trisN = 0;
	int vertsN = 0;




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
	Vec3d *vertsArr = new Vec3d[vertsN];

	ifs.read(&yn, 1); // cols 3 floats
	Vec3d *colorsArr = 0;
	if (yn == 'y') {
		colorsArr = new Vec3d[vertsN];
	}

	ifs.read(&yn, 1); // normals 3 floats
	Vec3d *normalsArr = 0;
	if (yn == 'y') {
		normalsArr = new Vec3d[vertsN];
	}

	ifs.read(&yn, 1); // texture coordinates 2 floats
	float *tcs = 0; // don't have texture coordinates for now
	if (tcs)
		delete tcs;
	tcs = 0;
	if (yn == 'y') {
		tcs = new float[vertsN * 2];
	}

	ifs.read((char*)vertsArr, vertsN * 3 * sizeof(float)); // load verts

	if (colorsArr) {
		ifs.read((char*)colorsArr, vertsN * 3 * sizeof(float)); // load cols
	}

	if (normalsArr)
		ifs.read((char*)normalsArr, vertsN * 3 * sizeof(float)); // load normals

	if (tcs)
		ifs.read((char*)tcs, vertsN * 2 * sizeof(float)); // load texture coordinates

	ifs.read((char*)&trisN, sizeof(int));
	unsigned int *trisArr = new unsigned int[trisN * 3];
	ifs.read((char*)trisArr, trisN * 3 * sizeof(unsigned int)); // read tiangles

	ifs.close();

	std::cerr << "INFO: loaded " << vertsN << " verts, " << trisN << " tris from " << endl << "      " << fname << endl;

	// copy into vectors
	std::copy(vertsArr, vertsArr + vertsN, std::back_inserter(verts));
	std::copy(colorsArr, colorsArr + vertsN, std::back_inserter(colors));
	std::copy(normalsArr, normalsArr + vertsN, std::back_inserter(normals));
	std::copy(trisArr, trisArr + trisN, std::back_inserter(tris));
	std::copy(trisArr, trisArr + trisN, std::back_inserter(normalTris));
	std::copy(trisArr, trisArr + trisN, std::back_inserter(texTris));
	texs.resize(verts.size());

	delete[] vertsArr;
	delete[] colorsArr;
	delete[] normalsArr;
	delete[] trisArr;
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
	verts.clear();
	normals.clear();
	texs.clear();
	tris.clear();
	texTris.clear();
	normalTris.clear();
	
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
			verts.push_back(vertex);
		}
		else if (strcmp(lineHeader, "vt") == 0)
		{
			Vec3d vertex;
			vertex[2] = 0;
			fscanf_s(file, "%f %f\n", &vertex[0], &vertex[1]);
			vertex[1] = 1.0f - vertex[1]; // flip the y uv because we support texture coords with the y flipped
			texs.push_back(vertex);
		}
		else if (strcmp(lineHeader, "vn") == 0)
		{
			Vec3d vertex;
			fscanf_s(file, "%f %f %f\n", &vertex[0], &vertex[1], &vertex[2]);
			normals.push_back(vertex);
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
			
			tris.push_back(vertexIndex[0] - 1);
			tris.push_back(vertexIndex[1] - 1);
			tris.push_back(vertexIndex[2] - 1);
			texTris.push_back(uvIndex[0] - 1);
			texTris.push_back(uvIndex[1] - 1);
			texTris.push_back(uvIndex[2] - 1);
			normalTris.push_back(normalIndex[0] - 1);
			normalTris.push_back(normalIndex[1] - 1);
			normalTris.push_back(normalIndex[2] - 1);
		}
	}
	colors.resize(verts.size());


	fclose(file);
	file = nullptr;

	cerr << "INFO: loaded " << verts.size() << " verts, " << tris.size() << " tris from " << endl << "      " << fname << endl;
}

#pragma endregion

Vec3d TMesh::GetCenter() const
{
	Vec3d ret(0.0f, 0.0f, 0.0f);
	for (int vi = 0; vi < verts.size(); vi++) {
		ret = ret + verts[vi];
	}
	ret = ret / (float)verts.size();
	return ret;
}

void TMesh::Translate(Vec3d tv) 
{
	for (int vi = 0; vi < verts.size(); vi++) {
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
	for (int vi = 0; vi < verts.size(); vi++)
	{
		verts[vi] = verts[vi].Rotate(axisOrigin, axisDir, theta);
	}
	for (int ni = 0; ni < normals.size(); ni++)
	{
		normals[ni] = normals[ni].Rotate(axisDir, theta);
	}
}

AABB TMesh::GetAABB() const
{
	if (verts.empty())
	{
		return AABB(Vec3d::ZEROS);
	}

	AABB aabb(verts[0]);

	for (int vi = 1; vi < verts.size(); vi++)
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
	for (int vi = 0; vi < verts.size(); vi++)
	{
		verts[vi] = verts[vi] * scaleFactor;
	}

	// Move back to original center
	SetCenter(currCenter);
}

void TMesh::ProjectAll(WorldView* view, bool invertW) 
{
	projected.resize(verts.size());

	// Project all of the verticies
	for (int i = 0; i < verts.size(); i++)
	{
		if (!view->GetPPC()->Project(verts[i], projected[i], invertW))
		{
			projected[i] = Vec3d(FLT_MAX, FLT_MAX, FLT_MAX);
		}
		else
		{
			// Clamp the projected points onto a 64x64 sub-grid 
			// This helps with numerical precision issues
			// If we're not inverting w, then we need to exclude Z from the clamping because it will be <1
			projected[i].Clamp(64, !invertW);
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
