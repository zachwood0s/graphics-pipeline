#include <iostream>
#include <algorithm>

#include "stdafx.h"

#include "framebuffer.h"
#include "math.h"
#include "scene.h"
#include "matrix3d.h"

#include <tiffio.h>

using namespace std;

FrameBuffer::FrameBuffer(int _w, int _h) 
{
	w = _w;
	h = _h;
	pix = new unsigned int[w*h];
	zb = new float[w*h];
	ids = new int[w*h];

}

#pragma region Get/Set

void FrameBuffer::ClearZB() 
{
	for (int uv = 0; uv < w*h; uv++)
	{
		zb[uv] = 0.0f;
		ids[uv] = -1;
	}
}

void FrameBuffer::Set(int u, int v, unsigned int color) 
{
	Set(u, v, color, -1);
}

void FrameBuffer::Set(int u, int v, unsigned int color, int id) 
{
	if (u < 0 || v < 0 || u > w - 1 || v > h - 1)
		return;

	int idx = (h - 1 - v)*w + u;
	pix[idx] = color;
	ids[idx] = id;
}

unsigned int FrameBuffer::Get(int u, int v)
{
	if (u < 0 || v < 0 || u > w - 1 || v > h - 1)
		return 0;

	return pix[(h - 1 - v)*w + u];
}

float FrameBuffer::GetZ(int u, int v)
{
	if (u < 0 || v < 0 || u > w - 1 || v > h - 1)
		return 0;

	return zb[(h - 1 - v)*w + u];
}

int FrameBuffer::GetId(int u, int v)
{
	if (u < 0 || v < 0 || u > w - 1 || v > h - 1)
		return 0;

	return ids[(h - 1 - v)*w + u];
}

void FrameBuffer::SetBGR(unsigned int bgr) 
{
	for (int uv = 0; uv < w*h; uv++)
		pix[uv] = bgr;

}

#pragma endregion

#pragma region Helper Functions

void ComputeBBox(Vec3d p1, Vec3d p2, Vec3d p3, float(&bbox)[2][2])
{
	bbox[0][0] = min(p1[0], min(p2[0], p3[0]));
	bbox[0][1] = max(p1[0], max(p2[0], p3[0]));
	bbox[1][0] = min(p1[1], min(p2[1], p3[1]));
	bbox[1][1] = max(p1[1], max(p2[1], p3[1]));
}

void ClipBBox(float(&bbox)[2][2], int u, int v, int w, int h)
{
	bbox[0][0] = max(bbox[0][0], (float) u);
	bbox[0][1] = min(bbox[0][1], (float) (u + w));
	bbox[1][0] = max(bbox[1][0], (float) v);
	bbox[1][1] = min(bbox[1][1], (float) (v + h));
}

Vec3d MakeEdge(Vec3d p1, Vec3d p2)
{
	Vec3d edge;
	edge[0] = p2[1] - p1[1]; 
	edge[1] = -p2[0] + p1[0]; 
	edge[2] = -(edge[0] * (p1[0] + p2[0]) + edge[1] * (p1[1] + p2[1])) / 2.0f;

	return edge;
}

Vec3d GetColorCoeffs(Matrix3d points, Vec3d rastParam)
{
	// Set all the z's to one
	points.SetColumn(2, Vec3d::ONES);

	return points.Inverted() * rastParam;
}


// load a tiff image to pixel buffer
bool FrameBuffer::LoadTiff(const char* fname) 
{
	TIFF* in = TIFFOpen(fname, "r");
	if (in == NULL) {
		cerr << fname << " could not be opened" << endl;
		return false;
	}

	bool success = true;

	int width, height;
	TIFFGetField(in, TIFFTAG_IMAGEWIDTH, &width);
	TIFFGetField(in, TIFFTAG_IMAGELENGTH, &height);
	if (w != width || h != height) {
		w = width;
		h = height;
		delete[] pix;
		pix = new unsigned int[w*h];
		glFlush();
		glFlush();
	}

	if (TIFFReadRGBAImage(in, w, h, pix, 0) == 0) {
		cerr << "failed to load " << fname << endl;
		success = false;
	}

	TIFFClose(in);
	return success;
}

// save as tiff image
void FrameBuffer::SaveAsTiff(const char *fname) 
{
	TIFF* out = TIFFOpen(fname, "w");

	if (out == NULL) {
		cerr << fname << " could not be opened" << endl;
		return;
	}

	TIFFSetField(out, TIFFTAG_IMAGEWIDTH, w);
	TIFFSetField(out, TIFFTAG_IMAGELENGTH, h);
	TIFFSetField(out, TIFFTAG_SAMPLESPERPIXEL, 4);
	TIFFSetField(out, TIFFTAG_BITSPERSAMPLE, 8);
	TIFFSetField(out, TIFFTAG_ORIENTATION, ORIENTATION_TOPLEFT);
	TIFFSetField(out, TIFFTAG_PLANARCONFIG, PLANARCONFIG_CONTIG);
	TIFFSetField(out, TIFFTAG_PHOTOMETRIC, PHOTOMETRIC_RGB);

	for (uint32 row = 0; row < (unsigned int)h; row++) {
		TIFFWriteScanline(out, &pix[(h - row - 1) * w], row);
	}

	TIFFClose(out);
}

int FrameBuffer::Farther(int u, int v, float currz) 
{
	if (u < 0 || u > w - 1 || v < 0 || v > h - 1)
		return 1;
	int uv = (h - 1 - v)*w + u;
	if (currz < zb[uv])
		return 1;
	zb[uv] = currz;
	return 0;
}

#pragma endregion

#pragma region Rasterization

void FrameBuffer::Draw2DSquarePoint(Vec3d p, int psize, Vec3d color) 
{
	int u = (int) p[0];
	int v = (int) p[1];
	unsigned int c = color.GetColor();

	for (int i = u - psize / 2; i <= u + psize / 2; i++) {
		for (int j = v - psize / 2; j <= v + psize / 2; j++) {
			Set(i, j, c);
		}
	}
}

void FrameBuffer::Draw3DSquarePoint(Vec3d p, PPC *ppc, int psize, Vec3d color) 
{
	Vec3d projected;
	if (!ppc->Project(p, projected))
		return;

	int u = (int) projected[0];
	int v = (int) projected[1];
	unsigned int c = color.GetColor();

	for (int cu = u - psize / 2; cu <= u + psize / 2; cu++) {
		for (int cv = v - psize / 2; cv <= v + psize / 2; cv++) {
			if (Farther(cu, cv, projected[2]))
				continue;
			Set(cu, cv, c);
		}
	}
}

void FrameBuffer::Draw2DPoint(Vec3d p, int r, Vec3d color)
{
	unsigned int c = color.GetColor();
	int u = (int)p[0];
	int v = (int)p[1];
	for (int i = -r; i < r; i++)
	{
		for (int j = -r; j < r; j++)
		{
			if (i * i + j * j < r*r)
			{
				Set(i + u, j + v, c);
			}
		}
	}
}


void FrameBuffer::Draw3DPoint(Vec3d p, PPC *ppc, int r, Vec3d color)
{
	Vec3d projected;
	if (!ppc->Project(p, projected))
		return;

	unsigned int c = color.GetColor();
	for (int i = -r; i < r; i++)
	{
		for (int j = -r; j < r; j++)
		{
			if (i * i + j * j < r*r)
			{
				int cu = i + (int) projected[0];
				int cv = j + (int) projected[1];
				if (Farther(cu, cv, projected[2]))
					continue;
				Set(cu, cv, c);
			}
		}
	}
}

void FrameBuffer::Draw2DTriangle(Vec3d p1, Vec3d p2, Vec3d p3, Vec3d c1, Vec3d c2, Vec3d c3, int id)
{
	// Clamp the projected points onto a 64x64 sub-grid 
	// This helps with numerical precision issues
	p1.Clamp(64); p2.Clamp(64); p3.Clamp(64);

	float area = ((p1 - p2) ^ (p2 - p3)).Length() / 2.0f;
	
	if (fabs(area) < 0.001f)
	{
		return;
	}

	// Matrix of [a, b, c] where a, b, c are vectors
	Matrix3d coefMatrix = Matrix3d(MakeEdge(p1, p2), MakeEdge(p2, p3), MakeEdge(p3, p1));

	Matrix3d points(p1, p2, p3);
	Vec3d reds(c1[0], c2[0], c3[0]);
	Vec3d greens(c1[1], c2[1], c3[1]);
	Vec3d blues(c1[2], c2[2], c3[2]);
	Vec3d zvals = points.GetColumn(2);
	Vec3d rCoefs = GetColorCoeffs(points, reds);
	Vec3d gCoefs = GetColorCoeffs(points, greens);
	Vec3d bCoefs = GetColorCoeffs(points, blues);
	Matrix3d colorCoefs = Matrix3d(rCoefs, gCoefs, bCoefs);
	Vec3d zCoefs = GetColorCoeffs(points, zvals);
	float minZ = zvals.Min();
	float maxZ = zvals.Max(); 
	Vec3d minColors(reds.Min(), greens.Min(), blues.Min());
	Vec3d maxColors(reds.Max(), greens.Max(), blues.Max());

	// Change the sign if the edges sidedness isn't right
	// I found this techique on a MIT lecture

	if (area < 0)
	{
		coefMatrix = coefMatrix * -1;
	}

	Vec3d a = coefMatrix.GetColumn(0);
	Vec3d b = coefMatrix.GetColumn(1);
	Vec3d c = coefMatrix.GetColumn(2);

	float bbox[2][2];
	ComputeBBox(p1, p2, p3, bbox);
	ClipBBox(bbox, 0, 0, w, h);

	int left = (int)(bbox[0][0] + .5), right = (int)(bbox[0][1] - .5);
	int top = (int)(bbox[1][0] + .5), bottom = (int)(bbox[1][1] - .5);

	// This is the same as t = a * left + b * top + c;
	Vec3d t = coefMatrix * Vec3d(left + .5f, top + .5f, 1);

	for (int currPixY = top; currPixY <= bottom; currPixY++, t = t + b)
	{
		int exit_early = 0; //Used for when we exit the triangle, we know we can continue onto next line because triangles are convex;
		Vec3d e = t;

		for (int currPixX = left; currPixX <= right; currPixX++, e = e + a)
		{
			if (e[0] >= 0 && e[1] >= 0 && e[2] >= 0)
			{
				Vec3d p = Vec3d((float) currPixX, (float) currPixY, 1);

				// Clamp the z value so that its valid for this triangle.
				float currZ = clamp(zCoefs * p, minZ, maxZ);
				if (Farther((int) p[0], (int) p[1], currZ))
				{
					continue;
				}
				Vec3d color = colorCoefs * p;

				// Clamp each color to somewhere in their starting range.
				// This handles errors for colors outside the given color range.
				color.Clamp(minColors, maxColors);

				Set(currPixX, currPixY, color.GetColor(), id);
				exit_early++;
			}
			else if (exit_early != 0)
			{
				break; // Continue onto next line
			}

		}
	}
}

void FrameBuffer::Draw3DTriangle(Vec3d P1, Vec3d P2, Vec3d P3, PPC *ppc, Vec3d c1, Vec3d c2, Vec3d c3, int id)
{
	Vec3d p1, p2, p3;
	if (!ppc->Project(P1, p1))
		return;
	if (!ppc->Project(P2, p2))
		return;
	if (!ppc->Project(P3, p3))
		return;

	Draw2DTriangle(p1, p2, p3, c1, c2, c3, id);
}

void FrameBuffer::Draw2DSegment(Vec3d p1, Vec3d p2, Vec3d c1, Vec3d c2) 
{
	float du = fabsf((p1 - p2)[0]);
	float dv = fabsf((p1 - p2)[1]);
	int stepsN;
	if (du < dv) {
		stepsN = 1+(int)dv;
	}
	else {
		stepsN = 1+(int)du;
	}
	for (int i = 0; i <= stepsN; i++) {
		Vec3d cp, cc;
		cp = p1 + (p2 - p1) * (float)i / (float)stepsN;
		// cp[2] depth (one over w) at current pixel
		int u = (int)cp[0], v = (int)cp[1];
		if (Farther(u, v, cp[2]))
			continue;
		cc = c1 + (c2 - c1) * (float)i / (float)stepsN;
		Set(u, v, cc.GetColor());
	}

}

void FrameBuffer::Draw3DSegment(Vec3d P1, Vec3d P2, PPC *ppc, Vec3d c1, Vec3d c2) 
{
	Vec3d p0, p1;
	if (!ppc->Project(P1, p0))
		return;
	if (!ppc->Project(P2, p1))
		return;

	Draw2DSegment(p0, p1, c1, c2);

}

#pragma endregion


#pragma region Texture Helpers

unsigned int FrameBuffer::GetTexVal(Vec3d texCoords)
{
	int u = (int) (texCoords[0] * w + .5f) % w;
	int v = h - ((int) (texCoords[1] * h + .5f) % h);

	return Get(u, v);
}

#pragma endregion