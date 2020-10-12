#pragma once
#include <vector>
#include <unordered_set>

#include "ppc.h"
#include "framebuffer.h"
#include "FLWindow.h"
#include "shaders.h"

class Scene;

class WorldView
{
private:
	PPC *ppc;
	FrameBuffer *fb;
	FLWindow *window;

public:
	int id;
	bool showCameraBox;
	bool showCameraScreen;
	float cameraVf;
	float kAmbient;
	Vec3d colorCameraBox;
	Vec3d colorCameraPoint;
	Vec3d background;

	std::vector<Shader> shaders;
	std::unordered_set<int> hiddenMeshes;

	/// <summary>
	/// Creates a world view with a window attached to it.
	/// </summary>
	WorldView(std::string name, int windowX, int windowY, int windowW, int windowH, float hFov, int _id);

	/// <summary>
	/// Creates a world view without a window attached.
	/// </summary>
	WorldView(int bufferW, int bufferH, float hfov, int _id);
	~WorldView();

	void Render(Scene &scene, bool disableLighting);
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

