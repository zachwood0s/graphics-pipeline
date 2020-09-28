#pragma once

#include "vec3d.h"

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
	AABB(std::initializer_list<Vec3d> a);

	void AddPoint(Vec3d point);
	void ClipView(int w, int h);
	Rect GetPixelRect();
};

