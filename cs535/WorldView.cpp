#include "WorldView.h"
#include "scene.h"



WorldView::WorldView(std::string name, int windowX, int windowY, int windowW, int windowH, float hFov, int _id)
	:id(_id), showCameraBox(true), showCameraScreen(true), cameraVf(20.0f)
{
	fb = new FrameBuffer(windowX, windowY, windowW, windowH, name.c_str());
	ppc = new PPC(hFov, windowW, windowH);

	fb->show();
	fb->redraw();

	colorCameraBox.SetFromColor(0xFFFF0000);
	colorCameraPoint.SetFromColor(0xFF0000FF);
	background.SetFromColor(0xFF000000);
}

WorldView::~WorldView()
{
	delete fb;
	delete ppc;
}

void WorldView::Render(Scene & scene)
{
	fb->SetBGR(background.GetColor());
	fb->ClearZB();

	for (int tmi = 0; tmi < scene.tmeshesN; tmi++) 
	{
		if (!scene.tmeshes[tmi].onFlag)
			continue;
		scene.tmeshes[tmi].DrawInterpolated(this, 0xFF00FF00);
	}

	if (showCameraBox || showCameraScreen)
	{
		// Go through each worldview and render their camera on this scene
		for (auto world : scene.views)
		{
			if (world->id == id) // Skip if its us
				continue;

			if (showCameraBox)
			{
				// Draw the camera boxes
				world->GetPPC()->Visualize(this, cameraVf, colorCameraBox, colorCameraPoint);
			}
			if (showCameraScreen)
			{
				// Draw the camera screens
				world->GetPPC()->Visualize(this, world, cameraVf);
			}
		}
	}

	fb->redraw();
}

PPC* WorldView::GetPPC()
{
	return ppc;
}

FrameBuffer* WorldView::GetFB()
{
	return fb;
}


#pragma region Render Functions

void WorldView::Draw2DSegment(Vec3d p1, Vec3d p2, Vec3d c1, Vec3d c2)
{
	fb->Draw2DSegment(p1, p2, c1, c2);
}

void WorldView::Draw3DSegment(Vec3d p1, Vec3d p2, Vec3d c1, Vec3d c2)
{
	fb->Draw3DSegment(p1, p2, ppc, c1, c2);
}

void WorldView::Draw2DTriangle(Vec3d p1, Vec3d p2, Vec3d p3, Vec3d c1, Vec3d c2, Vec3d c3, int id)
{
	fb->Draw2DTriangle(p1, p2, p3, c1, c2, c3, id);
}

void WorldView::Draw3DTriangle(Vec3d p1, Vec3d p2, Vec3d p3, Vec3d c1, Vec3d c2, Vec3d c3, int id)
{
	fb->Draw3DTriangle(p1, p2, p3, ppc, c1, c2, c3, id);
}

void WorldView::Draw2DPoint(Vec3d p, int radius, Vec3d color)
{
	fb->Draw2DPoint(p, radius, color);
}

void WorldView::Draw3DPoint(Vec3d p, int radius, Vec3d color)
{
	fb->Draw3DPoint(p, ppc, radius, color);
}

void WorldView::Draw2DSquarePoint(Vec3d p, int size, Vec3d color)
{
	fb->Draw2DSquarePoint(p, size, color);
}

void WorldView::Draw3DSquarePoint(Vec3d p, int size, Vec3d color)
{
	fb->Draw3DSquarePoint(p, ppc, size, color);
}

#pragma endregion
