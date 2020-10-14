#pragma once
#include <iostream>
#include <tuple>
#include "vec3d.h"

class PPC;

class Matrix3d
{
private:
	Vec3d rows[3];

public:
	static Matrix3d FromColumns(Vec3d c1, Vec3d c2, Vec3d c3);

	Matrix3d(Vec3d row1, Vec3d row2, Vec3d row3);
	Matrix3d(float vals[3][3]);
	Matrix3d();

	const Vec3d& operator[](const int& n) const;
	Vec3d& operator[](const int& n);
	Matrix3d operator*(const float& n) const;
	Matrix3d operator*(const Matrix3d& b) const;
	Vec3d operator*(const Vec3d& b) const;

	friend std::ostream& operator<<(std::ostream &output, const Matrix3d &m);
	friend std::istream& operator>>(std::istream &input, Matrix3d &m);

	std::tuple<Vec3d, Vec3d, Vec3d> Columns() const;

	Vec3d GetColumn(int column_idx) const;
	void SetColumn(int column_idx, const Vec3d value);
	bool IsValid() const;

	/// <summary>
	/// Returns the iverted version of this matrix
	/// </summary>
	/// <returns>The inverted version of this matrix</returns>
	Matrix3d Inverted() const;

	/// <summary>
	/// Returns the transposed version of this matrix
	/// </summary>
	/// <returns>The transposed version of this matrix</returns>
	Matrix3d Transposed() const;

	/// <summary>
	/// Sets this matrix as a rotation matrix about the x-axis. 
	/// </summary>
	/// <param name="theta">The rotation amount in degrees</param>
	void SetRotateX(const float theta);

	/// <summary>
	/// Sets this matrix as a rotation matrix about the y-axis. 
	/// </summary>
	/// <param name="theta">The rotation amount in degrees</param>
	void SetRotateY(const float theta);

	/// <summary>
	/// Sets this matrix as a rotation matrix about the z-axis. 
	/// </summary>
	/// <param name="theta">The rotation amount in degrees</param>
	void SetRotateZ(const float theta);

	/// <summary>
	/// Creates a screen space interpolation matrix from the matrix of 3D points (1 point for each triangle vertex);
	/// </summary>
	/// <param name="points">Matrix of triangle points</param>
	/// <returns>The screen space interpolation matrix</returns>
	static Matrix3d ScreenSpaceInterp(Matrix3d points);

	/// <summary>
	/// Creates a screen space interpolation matrix from the points of a triangle 
	/// </summary>
	/// <returns>The screen space interpolation matrix</returns>
	static Matrix3d ScreenSpaceInterp(Vec3d v1, Vec3d v2, Vec3d v3);

	/// <summary>
	/// Creates a model space interpolation matrix (perspecitvely correct) from the points of a triangle in matrix form.
	/// </summary>
	/// <param name="points">Matrix of triangle points</param>
	/// <param name="ppc">The camera of the current world view</param>
	/// <returns>The model space interpolation matrix</returns>
	static Matrix3d ModelSpaceInterp(Matrix3d points, PPC *ppc);

	/// <summary>
	/// Creates a model space interpolation matrix (perspecitvely correct) from the points of a triangle.
	/// </summary>
	/// <param name="ppc">The camera of the current world view</param>
	/// <returns>The model space interpolation matrix</returns>
	static Matrix3d ModelSpaceInterp(Vec3d v1, Vec3d v2, Vec3d v3, PPC *ppc);

	/// <summary>
	/// Sets up a matrix of edge equations from the points of a triangle in matrix form.
	/// </summary>
	/// <param name="points">Matrix of triangle points</param>
	/// <returns>A matrix containing the edge equations for a triangle</returns>
	static Matrix3d EdgeEquations(Matrix3d points);

	/// <summary>
	/// Sets up a matrix of edge equations from the points of a triangle.
	/// </summary>
	/// <returns>A matrix containing the edge equations for a triangle</returns>
	static Matrix3d EdgeEquations(Vec3d v1, Vec3d v2, Vec3d v3);

	/// <summary>
	/// Calculates the area of a triangle based the points in matrix form
	/// </summary>
	/// <param name="triangle">Matrix of triangle points</param>
	/// <returns>The area of the triangle</returns>
	static float TriangleArea(Matrix3d triangle);
};

