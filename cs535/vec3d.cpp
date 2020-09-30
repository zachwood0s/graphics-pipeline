#include <stdexcept>
#include <algorithm>

#include "stdafx.h"
#include "vec3d.h"
#include "matrix3d.h"
#include "AABB.h"


const Vec3d Vec3d::Vec3d::ONES = Vec3d(1.0f, 1.0f, 1.0f);
const Vec3d Vec3d::Vec3d::ZEROS = Vec3d(0.0f, 0.0f, 0.0f);
const Vec3d Vec3d::Vec3d::XAXIS = Vec3d(1.0f, 0.0f, 0.0f);
const Vec3d Vec3d::Vec3d::YAXIS = Vec3d(0.0f, 1.0f, 0.0f);
const Vec3d Vec3d::Vec3d::ZAXIS = Vec3d(0.0f, 0.0f, 1.0f);

Vec3d::Vec3d(float _x, float _y, float _z) : vals{_x, _y, _z}
{
}

Vec3d::Vec3d(float _vals[3]) : Vec3d(_vals[0], _vals[1], _vals[2])
{
}

Vec3d::Vec3d() : vals{0.0f, 0.0f, 0.0f}
{
}

Vec3d Vec3d::FromColor(unsigned int color)
{
	Vec3d ret; ret.SetFromColor(color);
	return ret;
}

Vec3d Vec3d::Normalized() const
{
	return (*this) / Length();
}

float Vec3d::Length() const
{
	const Vec3d& a = *this;
	return sqrtf(a * a);
}

float clamp(float val, float min, float max)
{
	return std::min(max, std::max(val, min));
}

void Vec3d::Clamp(AABB bounds)
{
	Clamp(bounds.Min(), bounds.Max());
}

void Vec3d::Clamp(Vec3d min, Vec3d max)
{
	Vec3d &v = *this;
	v[0] = clamp(v[0], min[0], max[0]);
	v[1] = clamp(v[1], min[1], max[1]);
	v[2] = clamp(v[2], min[2], max[2]);
}

void Vec3d::Clamp(int nearest)
{
	Vec3d &v = *this;
	v[0] = std::floorf(v[0] * nearest + .5f) / nearest;
	v[1] = std::floorf(v[1] * nearest + .5f) / nearest;
	v[2] = std::floorf(v[2] * nearest + .5f) / nearest;
}

float Vec3d::Min() const
{
	const Vec3d &v = *this;
	return std::min({ v[0], v[1], v[2] });
}

float Vec3d::Max() const
{
	const Vec3d &v = *this;
	return std::max({ v[0], v[1], v[2] });
}

std::tuple<float, float> Vec3d::Bounds() const
{
	return { Min(), Max() };
}

const float& Vec3d::operator[](const int& n) const
{
	return vals[n];
}

float& Vec3d::operator[](const int& n) 
{
	return vals[n];
}

Vec3d Vec3d::operator+(const Vec3d& b) const
{
	const Vec3d &a = *this;
	return Vec3d(a[0] + b[0], a[1] + b[1], a[2] + b[2]);
}

Vec3d Vec3d::operator-(const Vec3d& b) const
{
	const Vec3d &a = *this;
	return Vec3d(a[0] - b[0], a[1] - b[1], a[2] - b[2]);
}

float Vec3d::operator*(const Vec3d& b) const
{
	const Vec3d &a = *this;
	return a[0] * b[0] + a[1] * b[1] + a[2] * b[2];
}

Vec3d Vec3d::operator*(const float n) const
{
	const Vec3d &a = *this;
	return Vec3d(a[0] * n, a[1] * n, a[2] * n);
}

Vec3d Vec3d::operator^(const Vec3d& b) const
{
	const Vec3d &a = *this;
	return Vec3d(a[1]*b[2] - a[2] * b[1],
				 a[2]*b[0] - a[0] * b[2],
				 a[0]*b[1] - a[1] * b[0]);
}

Vec3d Vec3d::operator/(const float n) const
{
	const Vec3d &a = *this;
	return Vec3d(a[0] / n, a[1] / n, a[2] / n);
}

std::ostream& operator<<(std::ostream &output, const Vec3d &v)
{
	output << v[0] << " " << v[1] << " " << v[2];
	return output;
}

std::istream& operator>>(std::istream &input, Vec3d &v)
{
	input >> v[0] >> v[1] >> v[2];
	return input;
}

Vec3d Vec3d::Rotate(const Vec3d origin, const Vec3d direction, const float theta) const
{
	// Make new coordinate system
	Vec3d adir = direction;
	adir = adir.Normalized();
	Vec3d aux;
	if (fabsf(adir[0]) < fabsf(adir[1]))
	{
		aux = Vec3d::XAXIS;
	}
	else
	{
		aux = Vec3d::YAXIS; 
	}
	Vec3d a = adir;
	Vec3d b = (aux ^ adir).Normalized();
	Vec3d c = (a ^ b).Normalized();

	// Move into new coordinate system
	const Vec3d &P = *this;
	Matrix3d coord_rotation = Matrix3d(adir, b, c);
	Vec3d p_prime = coord_rotation * (P - origin);
	Matrix3d rotation_matrix = Matrix3d();
	rotation_matrix.SetRotateX(theta);

	// Apply rotation
	Vec3d p_rotated = rotation_matrix * p_prime;

	// Move out of coordinate system
	Vec3d p_transformed = (coord_rotation.Transposed() * p_rotated) + origin;
	return p_transformed;
}

Vec3d Vec3d::Rotate(const Vec3d direction, const float theta) const
{
	return Rotate(Vec3d::ZEROS, direction, theta);
}

void Vec3d::Set(const Vec3d other)
{
	vals[0] = other[0];
	vals[1] = other[1];
	vals[2] = other[2];
}

void Vec3d::SetFromColor(unsigned int color) 
{
	unsigned char *rgb = (unsigned char*)&color;
	Vec3d &v = *this;
	v[0] = (float)(rgb[0]) / 255.0f;
	v[1] = (float)(rgb[1]) / 255.0f;
	v[2] = (float)(rgb[2]) / 255.0f;
}

unsigned int Vec3d::GetColor() const
{
	unsigned int ret;
	const Vec3d &v = *this;
	unsigned char rgb[3];
	rgb[0] = (unsigned char)(255.0f * v[0]);
	rgb[1] = (unsigned char)(255.0f * v[1]);
	rgb[2] = (unsigned char)(255.0f * v[2]);
	ret = 0xFF000000 + rgb[2] * 256 * 256 + rgb[1] * 256 + rgb[0];
	return ret;
}

Vec3d Vec3d::Light(Vec3d lightVector, Vec3d normalVector, Vec3d viewDirection, float kAmbient, float kSpecular) const
{
	float kd = lightVector*normalVector; kd = (kd < 0.0f) ? 0.0f : kd;
	Vec3d reflected = (lightVector * -1).Reflect(normalVector);

	float kPhong = pow(std::max({ viewDirection * reflected, 0.0f }), 32) * kSpecular;
	const Vec3d &C = *this;
	return C*(kAmbient + (1.0f - kAmbient)*kd + kPhong);
}

Vec3d Vec3d::Reflect(Vec3d normal) const
{
	const Vec3d& d = *this;
	return normal * (-2 * (d * normal))  + d;
}

Vec3d Vec3d::Interpolate(Vec3d p0, Vec3d p1, int currStep, int stepCount)
{
	float scf = (float)currStep / (float)(stepCount - 1);
	return Interpolate(p0, p1, scf);
}

Vec3d Vec3d::Interpolate(Vec3d p0, Vec3d p1, float t)
{
	return p0 + (p1 - p0) * t;
}

Vec3d Vec3d::EdgeEquation(Vec3d v1, Vec3d v2, Vec3d v3)
{
#if 0

	(x - x0) / (x1 - x0) = (y - y0) / (y1 - y0)
	(x - x0)*(y1 - y0) = (y - y0)*(x1 - x0);
	x*(y1 - y0) + y * (x0 - x1) + x0(y0 - y1) + y0(x1 - x0) = 0
	xA + yB + C = 0
#endif
	float x0 = v1[0];
	float x1 = v2[0];
	float y0 = v1[1];
	float y1 = v2[1];

	Vec3d edge;
	edge[0] = y1 - y0; 
	edge[1] = -x1 + x0; 
	edge[2] = -(edge[0] * (x0 + x1) + edge[1] * (y0 + y1)) / 2.0f;

	Vec3d v2p(v3); v2p[2] = 1.0f;

	if (edge * v2p < 0.0f) // ret * v2p = Au + Bv + C
		edge = edge * -1.0f;

	return edge;
}
