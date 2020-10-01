#pragma once

#include <iostream>

class AABB;
class Material;

class Vec3d
{

private:
	float vals[3];


public:
	static const Vec3d XAXIS;
	static const Vec3d YAXIS;
	static const Vec3d ZAXIS;
	static const Vec3d ZEROS;
	static const Vec3d ONES;

	Vec3d(float x, float y, float z);
	Vec3d(float vals[3]);
	Vec3d();

	static Vec3d FromColor(unsigned int color);

	Vec3d Normalized() const;
	float Length() const;
	Vec3d Rotate(const Vec3d origin, const Vec3d direction, const float theta) const;
	Vec3d Rotate(const Vec3d direction, const float theta) const;
	void Set(const Vec3d other);
	void Clamp(int nearest);
	void Clamp(Vec3d minVals, Vec3d maxVals);
	void Clamp(AABB bounds);
	float Min() const;
	float Max() const;
	std::tuple<float, float> Bounds() const;

	const float& operator[](const int& n) const;
	float& operator[](const int& n);
	Vec3d operator+(const Vec3d&) const;
	Vec3d operator-(const Vec3d&) const;
	float operator*(const Vec3d&) const;
	Vec3d operator*(const float) const;
	Vec3d operator^(const Vec3d&) const;
	Vec3d operator/(const float) const;

	friend std::ostream& operator<<(std::ostream &output, const Vec3d &v);
	friend std::istream& operator>>(std::istream &input, Vec3d &v);

	void SetFromColor(unsigned int color);
	unsigned int GetColor() const;
	Vec3d Light(Vec3d lightVector, Vec3d normalVector, Vec3d viewDirection, float kAmbient, Material m) const;
	Vec3d LightBP(Vec3d lightVector, Vec3d normalVector, Vec3d half, float kAmbient, Material m) const;
	Vec3d Reflect(Vec3d normal) const;

	static Vec3d Interpolate(Vec3d p0, Vec3d p1, int currStep, int stepCount);
	static Vec3d Interpolate(Vec3d p0, Vec3d p1, float t);
	static Vec3d EdgeEquation(Vec3d v1, Vec3d v2, Vec3d other);

};

float clamp(float val, float min, float max);

