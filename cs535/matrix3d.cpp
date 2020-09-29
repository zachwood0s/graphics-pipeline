#define _USE_MATH_DEFINES
#include <math.h>

#include "matrix3d.h"
#include "ppc.h"



Matrix3d::Matrix3d(Vec3d row1, Vec3d row2, Vec3d row3): rows{row1, row2, row3}
{
}

Matrix3d::Matrix3d(float vals[3][3]) : rows{ Vec3d(vals[0]), Vec3d(vals[1]), Vec3d(vals[2]) }
{
}

Matrix3d::Matrix3d() : rows()
{
}

Matrix3d Matrix3d::FromColumns(Vec3d c1, Vec3d c2, Vec3d c3)
{
	Matrix3d m;
	m.SetColumn(0, c1);
	m.SetColumn(1, c2);
	m.SetColumn(2, c3);
	return m;
}

const Vec3d& Matrix3d::operator[](const int& n) const
{
	return rows[n];
}

Vec3d& Matrix3d::operator[](const int& n) 
{
	return rows[n];
}

Matrix3d Matrix3d::operator*(const float& n) const
{
	const Matrix3d& a = *this;
	Vec3d x = a[0] * n;
	Vec3d y = a[1] * n;
	Vec3d z = a[2] * n;
	return Matrix3d(x, y, z);
}

Matrix3d Matrix3d::operator*(const Matrix3d& b) const
{

	const Matrix3d& a = *this;
	Matrix3d res;
	
	res.SetColumn(0, a * b.GetColumn(0));
	res.SetColumn(1, a * b.GetColumn(1));
	res.SetColumn(2, a * b.GetColumn(2));

	return res;
}

Vec3d Matrix3d::operator*(const Vec3d& b) const
{
	const Matrix3d& a = *this;
	float x = a[0] * b;
	float y = a[1] * b;
	float z = a[2] * b;
	return Vec3d(x, y, z);
}

std::tuple<Vec3d, Vec3d, Vec3d> Matrix3d::Columns() const
{
	return { GetColumn(0), GetColumn(1), GetColumn(2) };
}

Vec3d Matrix3d::GetColumn(int column_idx) const
{
	const Matrix3d& a = *this;
	return Vec3d(a[0][column_idx], a[1][column_idx], a[2][column_idx]);
}

void Matrix3d::SetColumn(int column_idx, const Vec3d value)
{
	Matrix3d& a = *this;
	a[0][column_idx] = value[0];
	a[1][column_idx] = value[1];
	a[2][column_idx] = value[2];
}

Matrix3d Matrix3d::Inverted() const
{

	Matrix3d ret;
	Vec3d a = GetColumn(0), b = GetColumn(1), c = GetColumn(2);
	Vec3d _a = b ^ c; _a = _a / (a * _a);
	Vec3d _b = c ^ a; _b = _b / (b * _b);
	Vec3d _c = a ^ b; _c = _c / (c * _c);
	ret[0] = _a;
	ret[1] = _b;
	ret[2] = _c;

	return ret;
}

Matrix3d Matrix3d::Transposed() const
{
	return Matrix3d(GetColumn(0), GetColumn(1), GetColumn(2));
}

std::ostream& operator<<(std::ostream &output, const Matrix3d &v)
{
	output << v[0] << std::endl << v[1] << std::endl << v[2];
	return output;
}

std::istream& operator>>(std::istream &input, Matrix3d &v)
{
	input >> v[0] >> v[1] >> v[2];
	return input;
}

void Matrix3d::SetRotateX(const float thetaDegrees)
{
	const float theta = (thetaDegrees * (float) M_PI) / 180.0f;
	rows[0] = Vec3d::XAXIS; 
	rows[1] = Vec3d(0, cos(theta), -sin(theta));
	rows[2] = Vec3d(0, sin(theta), cos(theta));
}

void Matrix3d::SetRotateY(const float thetaDegrees)
{
	const float theta = (thetaDegrees * (float) M_PI) / 180.0f;
	rows[0] = Vec3d(cosf(theta), 0, sinf(theta));
	rows[1] = Vec3d::YAXIS;
	rows[2] = Vec3d(-sinf(theta), 0, cosf(theta));
}

void Matrix3d::SetRotateZ(const float thetaDegrees)
{
	const float theta = (thetaDegrees * (float) M_PI) / 180.0f;
	rows[0] = Vec3d(cos(theta), -sin(theta), 0);
	rows[1] = Vec3d(sin(theta), cos(theta), 0);
	rows[2] = Vec3d::ZAXIS;
}

Matrix3d Matrix3d::EdgeEquations(Matrix3d points)
{
	return EdgeEquations(points[0], points[1], points[2]);
}

Matrix3d Matrix3d::EdgeEquations(Vec3d v1, Vec3d v2, Vec3d v3)
{
	Vec3d pvs[3]{ v1, v2, v3 };
	Matrix3d ret;

	for (int ei = 0; ei < 3; ei++)
	{
		ret[ei] = Vec3d::EdgeEquation(pvs[ei], pvs[(ei + 1) % 3], pvs[(ei + 2) % 3]);
	}
	return ret;
}

Matrix3d Matrix3d::ScreenSpaceInterp(Matrix3d points)
{
	return ScreenSpaceInterp(points[0], points[1], points[2]);
}

Matrix3d Matrix3d::ScreenSpaceInterp(Vec3d v1, Vec3d v2, Vec3d v3)
{
	Matrix3d ret(v1, v2, v3);
	ret.SetColumn(2, Vec3d::ONES);
	return ret.Inverted();
}

Matrix3d Matrix3d::ModelSpaceInterp(Matrix3d points, PPC *ppc)
{
	return ModelSpaceInterp(points[0], points[1], points[2], ppc);
}

Matrix3d Matrix3d::ModelSpaceInterp(Vec3d v1, Vec3d v2, Vec3d v3, PPC *ppc)
{
	Matrix3d translatedPoints = Matrix3d::FromColumns(v1 - ppc->C, v2 - ppc->C, v3 - ppc->C);
	Matrix3d camera = Matrix3d::FromColumns(ppc->a, ppc->b, ppc->c);
	return translatedPoints.Inverted() * camera;
}

