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

	/// <summary>
	/// Returns a normalized version of the current vector
	/// </summary>
	/// <returns>A normalized version of the current vector</returns>
	Vec3d Normalized() const;

	/// <summary>
	/// Returns the magnitude of the vector
	/// </summary>
	/// <returns>The magnitude</returns>
	float Length() const;

	/// <summary>
	/// Returns the minimum value for the vector
	/// </summary>
	/// <returns>The minimum value</returns>
	float Min() const;

	/// <summary>
	/// Returns the maximum value for the vector
	/// </summary>
	/// <returns>The maximum value</returns>
	float Max() const;

	/// <summary>
	/// Retrieves the bounds of the vector in tuple form
	/// </summary>
	/// <returns>A tuple of the (min, max) of the vector</returns>
	std::tuple<float, float> Bounds() const;

	/// <summary>
	/// Checks if the vector is valid (values not equal to FLT_MAX)
	/// </summary>
	/// <returns>Whether or not the vector is valid</returns>
	bool IsValid() const;

	/// <summary>
	/// Rotates the vector around an axis given by the origin and direction of the axis.
	/// </summary>
	/// <param name="origin">The origin of the axis of rotation</param>
	/// <param name="direction">The direction of the axis of rotation</param>
	/// <param name="theta">The amount (in degrees) to rotate by</param>
	/// <returns>The rotated vector</returns>
	Vec3d Rotate(const Vec3d origin, const Vec3d direction, const float theta) const;

	/// <summary>
	/// Rotates the vector around an axis where the origin is [0,0,0]
	/// </summary>
	/// <param name="direction">The direction of the axis of rotation</param>
	/// <param name="theta">The amount (in degrees) to rotate by</param>
	/// <returns>The rotated vector</returns>
	Vec3d Rotate(const Vec3d direction, const float theta) const;

	/// <summary>
	/// Sets the current vectors values to those of the vector passed in
	/// </summary>
	/// <param name="other">The vector to copy the values of</param>
	void Set(const Vec3d other);

	/// <summary>
	/// Clamps each value in the vector to the nearest 1/nth value. 
	/// So a nearest value of 10 means the the vectors values would be rounded to the nearest tenths place.
	/// The z-value can be excluded if necessary (used during projections)
	/// </summary>
	/// <param name="nearest">The nearst 1/nth place to round to</param>
	/// <param name="excludeZ">Should the z-value be excluded from the clamping</param>
	void Clamp(int nearest, bool excludeZ = false);

	/// <summary>
	/// Clamps each value between the respective min and max values that are passed in
	/// </summary>
	/// <param name="minVals">The min values for each element</param>
	/// <param name="maxVals">The max values for each element</param>
	void Clamp(Vec3d minVals, Vec3d maxVals);

	/// <summary>
	/// Clamps each value between the minimum values of the bounding box and the maximum values of the bounding box
	/// </summary>
	/// <param name="bounds">A bounding box to clamp the vector into</param>
	void Clamp(AABB bounds);

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

	/// <summary>
	/// Sets the color values of this vector (between 0.0f and 1.0f) from a hex integer representing a color.
	/// </summary>
	/// <param name="color">Hex color</param>
	void SetFromColor(unsigned int color);

	/// <summary>
	/// Returns the hex color value of this vector
	/// </summary>
	/// <returns>The hex color value of this vector</returns>
	unsigned int GetColor() const;

	Vec3d Light(Vec3d lightVector, Vec3d normalVector, Vec3d viewDirection, float kAmbient, Material m) const;
	Vec3d Reflect(Vec3d normal) const;

	static Vec3d Interpolate(Vec3d p0, Vec3d p1, int currStep, int stepCount);
	static Vec3d Interpolate(Vec3d p0, Vec3d p1, float t);
	static Vec3d EdgeEquation(Vec3d v1, Vec3d v2, Vec3d other);

};

float clamp(float val, float min, float max);

