#pragma once

#include "ppc.h"
#include "framebuffer.h"

class Scene;

class WorldView
{
private:
	PPC *ppc;
	FrameBuffer *fb;
	int id;

public:
	bool showCameraBox;
	bool showCameraScreen;
	float cameraVf;
	Vec3d colorCameraBox;
	Vec3d colorCameraPoint;
	Vec3d background;

	WorldView(std::string name, int windowX, int windowY, int windowW, int windowH, float hFov, int _id);
	~WorldView();

	void Render(Scene &scene);
	PPC* GetPPC();
	FrameBuffer* GetFB();

#pragma region Render Functions

	void Draw2DSegment(Vec3d p1, Vec3d p2, Vec3d c1, Vec3d c2);
	void Draw3DSegment(Vec3d P1, Vec3d P2, Vec3d c1, Vec3d c2);
	void Draw2DTriangle(Vec3d p1, Vec3d p2, Vec3d p3, Vec3d c1, Vec3d c2, Vec3d c3);
	void Draw3DTriangle(Vec3d p1, Vec3d p2, Vec3d p3, Vec3d c1, Vec3d c2, Vec3d c3);
	void Draw2DPoint(Vec3d p, int radius, Vec3d color);
	void Draw3DPoint(Vec3d p, int radius, Vec3d color);
	void Draw2DSquarePoint(Vec3d p, int size, Vec3d color);
	void Draw3DSquarePoint(Vec3d p, int size, Vec3d color);

#pragma endregion
};

