#include <algorithm>

#include "AABB.h"



AABB::AABB(Vec3d firstPoint)
	:corners{ firstPoint, firstPoint }
{
}

AABB AABB::FromPoints(std::initializer_list<Vec3d> list)
{
	AABB aabb(*list.begin());
	for (auto v : list)
	{
		aabb.AddPoint(v);
	}
	return aabb;
}

AABB AABB::FromMatrixColumns(Matrix3d m)
{
	auto[c1, c2, c3] = m.Columns();
	return AABB::FromPoints({ c1, c2, c3 });
}

AABB AABB::Clipped(Rect bounds, std::initializer_list<Vec3d> list)
{
	AABB aabb = AABB::FromPoints(list);
	aabb.ClipView(bounds);
	return aabb;
}

void AABB::AddPoint(Vec3d point)
{
	for (int i = 0; i < 3; i++)
	{
		if (corners[0][i] > point[i])
			corners[0][i] = point[i];
		if (corners[1][i] < point[i])
			corners[1][i] = point[i];
	}
}

void AABB::ClipView(Rect bounds)
{
	corners[0][0] = clamp(corners[0][0], (float) bounds.left, (float) bounds.right); 
	corners[0][1] = clamp(corners[0][1], (float) bounds.top, (float) bounds.bottom); 
	corners[1][0] = clamp(corners[1][0], (float) bounds.left, (float) bounds.right); 
	corners[1][1] = clamp(corners[1][1], (float) bounds.top, (float) bounds.bottom); 
}

Rect AABB::GetPixelRect() const
{
	Rect r;
	r.left = (int)(corners[0][0] + .5f);
	r.right = (int)(corners[1][0] - .5f);
	r.top = (int)(corners[0][1] + .5f);
	r.bottom = (int)(corners[1][1] - .5f);
	return r;
}

Vec3d AABB::Min() const
{
	return corners[0];
}

Vec3d AABB::Max() const
{
	return corners[1];
}
