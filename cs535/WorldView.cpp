
#include "WorldView.h"
#include "scene.h"




WorldView::WorldView(std::string name, int windowX, int windowY, int windowW, int windowH, float hFov, int _id)
	: WorldView(windowW, windowH, hFov, _id)
{
	window = std::make_unique<FLWindow>(windowX, windowY, fb.get(), name.c_str());

	window->show();
	window->redraw();
}

WorldView::WorldView(int bufferW, int bufferH, float hFov, int _id) 
	: id(_id), showCameraBox(false), showCameraScreen(false), cameraVf(20.0f), window(nullptr), kAmbient(0.2f)
	, fb(std::make_unique<FrameBuffer>(bufferW, bufferH)), ppc(std::make_unique<PPC>(hFov, bufferW, bufferH))
{
	colorCameraBox.SetFromColor(0xFFFF0000);
	colorCameraPoint.SetFromColor(0xFF0000FF);
	background.SetFromColor(0xFF000000);

	backgroundShader = Shaders::solidBackground;
}

void WorldView::Render(Scene & scene, bool disableLighting)
{
	backgroundShader(scene, *this);
	fb->ClearZB();

	for (int tmi = 0; tmi < scene.tmeshes.size(); tmi++) 
	{
		if (!scene.tmeshes[tmi]->onFlag || hiddenMeshes.count(tmi))
			continue;

		scene.tmeshes[tmi]->DrawModelSpaceInterpolated(scene, this, disableLighting);
	}

	if (showCameraBox || showCameraScreen)
	{
		// Go through each worldview and render their camera on this scene
		for (auto& world : scene.views)
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
				world->GetPPC()->Visualize(this, world.get(), cameraVf);
			}
		}
		if (scene.cubeMap)
		{
			for (auto& world : scene.cubeMap->views)
			{
				if (showCameraBox)
				{
					// Draw the camera boxes
					world->GetPPC()->Visualize(this, cameraVf, colorCameraBox, colorCameraPoint);
				}
				if (showCameraScreen)
				{
					// Draw the camera boxes
					world->GetPPC()->Visualize(this, world.get(), cameraVf);
				}
			}
		}
	}
	for (auto& light : scene.lights) Draw3DPoint(light->GetCenter(), 5, Vec3d::XAXIS);
	for (auto& proj : scene.projectors) Draw3DPoint(proj->GetCenter(), 5, Vec3d::YAXIS);

	Redraw();
}

void WorldView::Redraw()
{
	// If a window is associated with this world, redraw it
	if (window)
	{
		window->redraw();
	}
}

PPC* WorldView::GetPPC()
{
	return ppc.get();
}

FrameBuffer* WorldView::GetFB()
{
	return fb.get();
}


#pragma region Render Functions

void WorldView::Draw2DSegment(Vec3d p1, Vec3d p2, Vec3d c1, Vec3d c2)
{
	fb->Draw2DSegment(p1, p2, c1, c2);
}

void WorldView::Draw3DSegment(Vec3d p1, Vec3d p2, Vec3d c1, Vec3d c2)
{
	fb->Draw3DSegment(p1, p2, ppc.get(), c1, c2);
}

void WorldView::Draw2DTriangle(Vec3d p1, Vec3d p2, Vec3d p3, Vec3d c1, Vec3d c2, Vec3d c3, int id)
{
	fb->Draw2DTriangle(p1, p2, p3, c1, c2, c3, id);
}

void WorldView::Draw3DTriangle(Vec3d p1, Vec3d p2, Vec3d p3, Vec3d c1, Vec3d c2, Vec3d c3, int id)
{
	fb->Draw3DTriangle(p1, p2, p3, ppc.get(), c1, c2, c3, id);
}

void WorldView::Draw2DPoint(Vec3d p, int radius, Vec3d color)
{
	fb->Draw2DPoint(p, radius, color);
}

void WorldView::Draw3DPoint(Vec3d p, int radius, Vec3d color)
{
	fb->Draw3DPoint(p, ppc.get(), radius, color);
}

void WorldView::Draw2DSquarePoint(Vec3d p, int size, Vec3d color)
{
	fb->Draw2DSquarePoint(p, size, color);
}

void WorldView::Draw3DSquarePoint(Vec3d p, int size, Vec3d color)
{
	fb->Draw3DSquarePoint(p, ppc.get(), size, color);
}

#pragma endregion
