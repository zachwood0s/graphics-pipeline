#pragma once

#include "vec3d.h"
#include "matrix3d.h"

struct Rect {
	int left;
	int right;
	int top;
	int bottom;
};

class AABB
{
public:
	Vec3d corners[2];
	AABB(Vec3d firstPoint);

	static AABB Clipped(int w, int h, std::initializer_list<Vec3d> points);
	static AABB FromPoints(std::initializer_list<Vec3d> points);
	static AABB FromMatrixColumns(Matrix3d m);

	void AddPoint(Vec3d point);
	void ClipView(int w, int h);

	Rect GetPixelRect() const;
	Vec3d Min() const;
	Vec3d Max() const;
};

