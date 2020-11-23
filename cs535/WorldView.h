#pragma once
#include <vector>
#include <unordered_set>

#include "ppc.h"
#include "framebuffer.h"
#include "FLWindow.h"
#include "shaders.h"

class Scene;


/// <summary>
/// A world view is essentially a framebuffer with an attached camera. A few of the framebuffer drawing
/// functions have been ported into this class to make them easier to use without haveing to pass the camera
/// and framebuffer into each. 
/// 
/// This class can also have a window associated with it if a name and window position are provided in the constructor.
/// Without those parameters it essentially acts as a 'fancy' frame buffer.
/// </summary>
class WorldView
{
public:
	enum RENDER_MODE
	{
		Software,
		Hardware,
		GPU
	};

private:
	std::unique_ptr<PPC> ppc;
	std::unique_ptr<FrameBuffer> fb;
	std::unique_ptr<FLWindow> window;
	RENDER_MODE mode;

public:

	int id;
	bool showCameraBox;
	bool showCameraScreen;
	float cameraVf;
	float kAmbient;
	Vec3d colorCameraBox;
	Vec3d colorCameraPoint;
	Vec3d background;

	/// <summary>
	/// These shaders describe what should happen for each pixel after the interpolation of a mesh has happened
	/// </summary>
	std::vector<Shader> shaders;

	/// <summary>
	/// This shader draws the background before anything else. Can be used for environment mapping of background drawing
	/// </summary>
	BackgroundShader backgroundShader;

	/// <summary>
	/// A set of mesh indices that should not be rendered in this view.
	/// </summary>
	std::unordered_set<int> hiddenMeshes;

	/// <summary>
	/// Creates a world view with a window attached to it.
	/// </summary>
	WorldView(std::string name, int windowX, int windowY, int windowW, int windowH, float hFov, int _id);

	/// <summary>
	/// Creates a world view without a window attached.
	/// </summary>
	WorldView(int bufferW, int bufferH, float hfov, int _id);

	void Render(Scene &scene, bool disableLighting);
	void RenderSW(Scene &scene, bool disableLighting);
	void RenderHW(Scene &scene);
	void RenderGPU(Scene &scene);

	void SetMode(RENDER_MODE mode);

	void Redraw();
	PPC* GetPPC();
	FrameBuffer* GetFB();

#pragma region Render Functions

	void Draw2DSegment(Vec3d p1, Vec3d p2, Vec3d c1, Vec3d c2);
	void Draw3DSegment(Vec3d P1, Vec3d P2, Vec3d c1, Vec3d c2);
	void Draw2DTriangle(Vec3d p1, Vec3d p2, Vec3d p3, Vec3d c1, Vec3d c2, Vec3d c3, int id = -1);
	void Draw3DTriangle(Vec3d p1, Vec3d p2, Vec3d p3, Vec3d c1, Vec3d c2, Vec3d c3, int id = -1);
	void Draw2DPoint(Vec3d p, int radius, Vec3d color);
	void Draw3DPoint(Vec3d p, int radius, Vec3d color);
	void Draw2DSquarePoint(Vec3d p, int size, Vec3d color);
	void Draw3DSquarePoint(Vec3d p, int size, Vec3d color);

#pragma endregion
};

