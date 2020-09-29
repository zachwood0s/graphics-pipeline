#pragma once

#include <FL/Fl.H>
#include <FL/Fl_Gl_Window.H>
#include <GL/glut.h>

#include "vec3d.h"
#include "ppc.h"

class FrameBuffer{
public:
	unsigned int *pix; // pixel array
	float *zb;
	int *ids; // ids of the triangle that set the pixel

	int w, h;
	FrameBuffer(int _w, int _h);

	void SetBGR(unsigned int bgr);
	void Set(int u, int v, unsigned int color);
	void Set(int u, int v, unsigned int color, int id);

#pragma region Drawing Functions

	/// <summary>
	/// Draws a 2D triangle with the given endpoints and corresponding colors. 
	/// An optional triangle id may be passed in. This is used to trace triangles malformed/missing 
	/// triangles on a mesh.
	/// </summary>
	/// <param name="id">The optional triangle id</param>
	void Draw2DTriangle(Vec3d p1, Vec3d p2, Vec3d p3, Vec3d c1, Vec3d c2, Vec3d c3, int id = -1);

	/// <summary>
	/// Draws a 3D triangle with the given endpoints and corresponding colors. The points are projected
	/// using the provided PPC and then drawn with the 2D function. An optional triangle id may be passed in. 
	/// This is used to trace triangles malformed/missing triangles on a mesh.
	/// </summary>
	/// <param name="id">The optional triangle id</param>
	void Draw3DTriangle(Vec3d P1, Vec3d P2, Vec3d P3, PPC *ppc, Vec3d c1, Vec3d c2, Vec3d c3, int id = -1);

	/// <summary>
	/// Draws a 2D lines segment between the provided points with the given colors.
	/// </summary>
	void Draw2DSegment(Vec3d p0, Vec3d p1, Vec3d c0, Vec3d c1);

	/// <summary>
	/// Draws a 3D line segment between the provided 3D points with the given colors. The points are projected
	/// using the provided PPC and then drawn with the 2D segment function.
	/// </summary>
	void Draw3DSegment(Vec3d P0, Vec3d P1, PPC *ppc, Vec3d c0, Vec3d c1);

	/// <summary>
	/// Draws a square at the provided point with the provided size and color.
	/// </summary>
	/// <param name="p">The center point of the square</param>
	/// <param name="psize">The side length of the square</param>
	/// <param name="color">The color of the square</param>
	void Draw2DSquarePoint(Vec3d p, int psize, Vec3d color);

	/// <summary>
	/// Draws a square at the provided 3D point with the provided size and color. The point
	/// is projected using the provided PPC and then drawn with the 2D function.
	/// Note: the size is not projected. So points will be drawn at a constant size regardless
	/// of their 3D position
	/// </summary>
	void Draw3DSquarePoint(Vec3d p, PPC *ppc, int psize, Vec3d color);

	/// <summary>
	/// Draws a circular point at the provided point with the provided radius and color.
	/// </summary>
	/// <param name="p">The center point of the circle</param>
	/// <param name="radius">The radius of the circle</param>
	/// <param name="color">The color of the circle</param>
	void Draw2DPoint(Vec3d p, int radius, Vec3d color);

	/// <summary>
	/// Draws a circular point at the provided 3D point with the provided radius and color.
	/// The point is projected using the provided PPC and then drawn with the 2D function. 
	/// Note: the radius is not projected, so points will be drawn at a constant size regardless 
	/// of their 3D position.
	/// </summary>
	void Draw3DPoint(Vec3d p, PPC *ppc, int radius, Vec3d color);

#pragma endregion

#pragma region Helper Functions

	/// <summary>
	/// Checks to see if the passed in value is farther away than the one in the z-buffer.
	/// Returns 0 if the provided z value is closer and updates the z-buffer with the given point. Otherwise,
	/// returns 1 indicating that this value is smaller than the one in the z-buffer.
	/// </summary>
	/// <param name="u">The screen x coordinate to check</param>
	/// <param name="v">The screen y coordinate to check</param>
	/// <param name="currz">The z-value to check with</param>
	/// <returns>0 if z-value is closer than the one in the z-buffer, 1 if farther.</returns>
	int Farther(int u, int v, float currz);

	/// <summary>
	/// Gets the z-buffer value for a given point
	/// </summary>
	/// <param name="u">The screen x coordinate to check</param>
	/// <param name="v">The screen y coordinate to check</param>
	/// <returns>The z-buffer value for a given point</returns>
	float GetZ(int u, int v);

	/// <summary>
	/// Gets the pixel value (color) for a given point.
	/// </summary>
	/// <param name="u">The screen x coordinate to check</param>
	/// <param name="v">The screen y coordinate to check</param>
	/// <returns>The pixel value (color) for a given screen coordinate</returns>
	unsigned int Get(int u, int v);

	/// <summary>
	/// Gets the triangle id associated with the given screen coordinate.
	/// </summary>
	/// <param name="u">The screen x coordinate to check</param>
	/// <param name="v">The screen y coordinate to check</param>
	/// <returns>The triangle id associated with the given screen coordinate</returns>
	int GetId(int u, int v);

	/// <summary>
	/// Sets the pixel values to those found in the provided .tiff file.
	/// </summary>
	/// <param name="fname">The .tiff file to load into the frame buffer</param>
	void LoadTiff(char* fname);

	/// <summary>
	/// Saves the current pixel values to a .tiff image file.
	/// </summary>
	/// <param name="fname">The .tiff file to save the buffer to</param>
	void SaveAsTiff(const char *fname);

	/// <summary>
	/// Clears the z-buffer by setting all of the values to 0.
	/// </summary>
	void ClearZB();

#pragma endregion

};