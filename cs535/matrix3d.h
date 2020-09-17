#pragma once
#include <iostream>
#include "vec3d.h"

class Matrix3d
{
private:
	Vec3d rows[3];

public:
	Matrix3d(Vec3d row1, Vec3d row2, Vec3d row3);
	Matrix3d(float vals[3][3]);
	Matrix3d();

	const Vec3d& operator[](const int& n) const;
	Vec3d& operator[](const int& n);
	Matrix3d operator*(const float& n);
	Matrix3d operator*(const Matrix3d& b);
	Vec3d operator*(const Vec3d& b);

	friend std::ostream& operator<<(std::ostream &output, const Matrix3d &m);
	friend std::istream& operator>>(std::istream &input, Matrix3d &m);

	Vec3d GetColumn(int column_idx) const;
	void SetColumn(int column_idx, const Vec3d value);
	Matrix3d Inverted();
	Matrix3d Transposed();

	void SetRotateX(const float theta);
	void SetRotateY(const float theta);
	void SetRotateZ(const float theta);

};

