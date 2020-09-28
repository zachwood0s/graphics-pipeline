#pragma once


#include "matrix3d.h"
#include "vec3d.h"

class WorldView;

class PPC {
private:
	Matrix3d invertedCached;

	void CacheInverted();

public:
	Vec3d a, b, c, C;
	int w, h;
	PPC(float hfov, int _w, int _h);

	void ReadFromFile(std::string fname);

	void WriteToFile(std::string fname);

	/// <summary>
	/// Sets the cameras position, look at point, and "up guidance". 
	/// </summary>
	/// <param name="newEye">The cameras new position</param>
	/// <param name="lookAtPoint">The point the camera should look at</param>
	/// <param name="upGuidance">The 'up' direction</param>
	void SetPose(Vec3d newEye, Vec3d lookAtPoint, Vec3d upGuidance);

	/// <summary>
	/// Sets the focal length of the camera
	/// </summary>
	void SetFocalLength(float focal);

	/// <summary>
	/// Scales the focal length by the given zoom factor
	/// </summary>
	void ZoomFocalLength(float zoom);

	/// <summary>
	/// Projects a point using this camera
	/// </summary>
	/// <param name="P">The unprojected, 3D point</param>
	/// <param name="p">The modified, projected, 2D point</param>
	/// <returns>1 if successful, 0 if not</returns>
	int Project(Vec3d P, Vec3d &p);

	/// <summary>
	/// Unprojects a point using this camera
	/// </summary>
	/// <param name="p">The projected, 2D point to unproject</param>
	/// <returns>The unprojected, 3D point</returns>
	Vec3d UnProject(Vec3d p);

	/// <summary>
	/// Translates the camera to the left/right by the given amount. Left is negative, right is positive.
	/// </summary>
	void TranslateRightLeft(float tstep);

	/// <summary>
	/// Translates the camera forward/backward by the given amount. Front is positive, back is negative.
	/// </summary>
	void TranslateFrontBack(float tstep);

	/// <summary>
	/// Translates the camera up/down by the given amount. Up is positive, left is negative
	/// </summary>
	void TranslateUpDown(float tstep);

	/// <summary>
	/// Pans the camera left/right by the given amount in degrees. Left is negative, right is positive.
	/// </summary>
	void PanLeftRight(float rstep);

	/// <summary>
	/// Tilts the camera up/down by the given amount in degrees. Up is positive, down is negative.
	/// </summary>
	void TiltUpDown(float rstep);

	/// <summary>
	/// Rolls the camera by the given amount. Right is positive, left is negative
	/// </summary>
	void Roll(float rstep);

	/// <summary>
	/// Retrieves the focal length of the camera
	/// </summary>
	/// <returns>The focal length of the camera</returns>
	float GetFocalLength();

	/// <summary>
	/// Interpolates this camera between the two provided cameras. This interpolates the postition,
	/// look at point and upguidance vectors.
	/// </summary>
	/// <param name="ppc0">The starting camera</param>
	/// <param name="pp1">The ending camera</param>
	/// <param name="currStep">The current step of interpolation</param>
	/// <param name="numSteps">The total number of steps in the interpolation</param>
	void Interpolate(PPC* ppc0, PPC* pp1, int currStep, int numSteps);

	/// <summary>
	/// Retrieves the principal point in the camera (the center of the cameras screen in 3D space).
	/// </summary>
	/// <returns>The camera's principal point</returns>
	Vec3d GetPrincipal();

	/// <summary>
	/// Retrieves the view direction of the camera.
	/// </summary>
	Vec3d GetViewDirection();

	/// <summary>
	/// Draws the camera's frustrum and C point on the given WorldView. 
	/// </summary>
	/// <param name="world">The world to draw the camera on</param>
	/// <param name="vf">The scale factor for the focal length to make it more reasonable</param>
	/// <param name="colorBox">The color to draw the end of the frustrum with</param>
	/// <param name="colorPoint">The color to draw the camera's point with</param>
	void Visualize(WorldView *world, float vf, Vec3d colorBox, Vec3d colorPoint);

	/// <summary>
	/// Draws a WorldView on the viewplane of the cameras frustrum. Can be used to show what a camera
	/// is seeing on the camera's frustrum.
	/// </summary>
	/// <param name="world">The world to draw on</param>
	/// <param name="cameraWorld">The world to draw from</param>
	/// <param name="vf">The scale factor for the focal length to make it more reasonable</param>
	void Visualize(WorldView *world, WorldView *cameraWorld, float vf);

};