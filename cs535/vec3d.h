#pragma once
#include <iostream>

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

	Vec3d Normalized();
	float Length();
	Vec3d Rotate(const Vec3d origin, const Vec3d direction, const float theta);
	Vec3d Rotate(const Vec3d direction, const float theta);
	void Set(const Vec3d other);
	void Clamp(int nearest);
	void Clamp(Vec3d minVals, Vec3d maxVals);
	float Min();
	float Max();

	const float& operator[](const int& n) const;
	float& operator[](const int& n);
	Vec3d operator+(const Vec3d&);
	Vec3d operator-(const Vec3d&);
	float operator*(const Vec3d&);
	Vec3d operator*(const float);
	Vec3d operator^(const Vec3d&);
	Vec3d operator/(const float);

	friend std::ostream& operator<<(std::ostream &output, const Vec3d &v);
	friend std::istream& operator>>(std::istream &input, Vec3d &v);

	void SetFromColor(unsigned int color);
	unsigned int GetColor();

	static Vec3d Interpolate(Vec3d p0, Vec3d p1, int currStep, int stepCount);
	static Vec3d Interpolate(Vec3d p0, Vec3d p1, float t);

};

float clamp(float val, float min, float max);

