#include <chrono>
#include "stdafx.h"

#include "matrix3d.h"
#include "vec3d.h"
#include "scene.h"
#include "shaders.h"

Scene *scene;

using namespace std;

#include <iostream>

Scene::Scene() 
{
	gui = new GUI();
	gui->show();

	int u0 = 20;
	int v0 = 20;
	int h = 480;
	int w = 640;
	float hfov = 55.0f;

	Vec3d cameraStart(0.0f, 50.0f, 200.0f);

	// Create the stationary camera world view
	WorldView * world = new WorldView("Audience Without the Effect", u0, v0 + h + 20, w, h, hfov, (int) views.size());
	world->background.SetFromColor(0xFFFFFFFF);
	world->kAmbient = .05f;
	world->GetPPC()->ZoomFocalLength(.5f);
	world->GetPPC()->SetPose(cameraStart, Vec3d::ZEROS, Vec3d::YAXIS);

	world->shaders.push_back(Shaders::invisibilityShader);

	views.push_back(world);

	// Create the moving camera world view
	world = new WorldView("Audience", u0, v0, w, h, hfov, (int) views.size());
	world->background.SetFromColor(0xFFFFFFFF);
	world->GetPPC()->ZoomFocalLength(.5f);
	world->kAmbient = .05f;
	world->GetPPC()->SetPose(cameraStart, Vec3d::ZEROS, Vec3d::YAXIS);

	world->shaders.push_back(Shaders::projectiveTextures);

	views.push_back(world);

	// This won't be shown as a screen
	world = new WorldView(w, h, hfov, (int) views.size());
	world->background.SetFromColor(0xFFFFFFFF);
	world->kAmbient = .05f;
	world->GetPPC()->ZoomFocalLength(.5f);
	world->GetPPC()->SetPose(cameraStart, Vec3d::ZEROS, Vec3d::YAXIS);

	world->hiddenMeshes.insert(0);

	views.push_back(world);

	world = new WorldView("Render", 0, 0, w*3, h, hfov, (int) views.size());
	world->background.SetFromColor(0xFFFFFFFF);
	world->kAmbient = .05f;
	world->GetPPC()->ZoomFocalLength(.5f);
	world->GetPPC()->SetPose(cameraStart, Vec3d::ZEROS, Vec3d::YAXIS);

	world->hiddenMeshes.insert(0);

	views.push_back(world);

	gui->uiw->position(u0 + w, v0 + h + 50);

	tmeshesN = 6;
	tmeshes = new TMesh[tmeshesN];

	TEX_HANDLE head = LoadTexture("textures/fox_head_color.tiff", false);
	TEX_HANDLE checker = LoadTexture("textures/checker.tiff", false);
	TEX_HANDLE mountains = LoadTexture("textures/mountains.tiff", true);
	TEX_HANDLE hole = LoadTexture("textures/hole.tiff", false);
	TEX_HANDLE crate = LoadTexture("textures/crate.tiff", false);
	TEX_HANDLE mirror = LoadTexture("textures/mirror.tiff", false);
	TEX_HANDLE hello = LoadTexture("textures/hellotext.tiff", false);


	// left cube
	tmeshes[0].LoadObj("geometry/cube.obj");
	tmeshes[0].ScaleTo(60);
	tmeshes[0].SetCenter(Vec3d(-80, 30, 40));
	tmeshes[0].SetMaterial({ Vec3d::ONES * .1f, 32, 0.1f, checker });
	tmeshes[0].onFlag = true;

	// Bottom plane
	tmeshes[1].LoadObj("geometry/plane.obj");
	tmeshes[1].ScaleTo(600);
	tmeshes[1].SetCenter(Vec3d(0.0f, 0.0f, -70.0f));
	tmeshes[1].SetMaterial({ Vec3d::ONES * .1f, 32, 0.9f, checker });
	tmeshes[1].onFlag = true;

	// Back plane
	tmeshes[2].LoadObj("geometry/plane.obj");
	tmeshes[2].ScaleTo(600);
	tmeshes[2].Rotate(Vec3d::ZEROS, Vec3d::XAXIS, 90.0f);
	tmeshes[2].SetCenter(Vec3d(0.0f, 0.0f, -40.0f));
	tmeshes[2].SetMaterial({ Vec3d::YAXIS * .1f, 8, 0.5f, mountains });
	tmeshes[2].onFlag = true;

	tmeshes[3].LoadObj("geometry/fox.obj");
	tmeshes[3].ScaleTo(100);
	tmeshes[3].SetCenter(Vec3d(0.0f, 30.0f, -0.0f));
	tmeshes[3].SetMaterial({ Vec3d::ONES * .1f, 32, 0.9f, head });
	tmeshes[3].onFlag = true;

	tmeshes[4].LoadObj("geometry/cube.obj");
	tmeshes[4].ScaleTo(110);
	tmeshes[4].SetCenter(Vec3d(-5.0f, 30, -0));
	tmeshes[4].SetMaterial({ Vec3d::ONES * .1f, 32, 0.1f, hole });
	tmeshes[4].onFlag = true;

	

	//Light * light = new Light(Vec3d(-140, 254, 140), 800, 800, 90.0f);
	//light->SetAttenuation(1.0f, 0.00000045f, 0.000000075f);
	//lights.push_back(light);

	//light = new Light(Vec3d(-140, 104, 140), 800, 800, 90.0f);
	//light->SetAttenuation(1.0f, 0.00000045f, 0.000000075f);
	//lights.push_back(light);


	Projector * projector = new Projector(Vec3d(50, 100, 230), w, h, 90.0f, TEX_INVALID);
	projector->shadowMap->shaders.pop_back();
	projectors.push_back(projector);
	//views[1]->GetPPC()->SetPose(Vec3d(200, 0, -300), Vec3d::ZEROS, Vec3d::YAXIS);


	Render();

}

Scene::~Scene()
{
	for (auto world : views)
	{
		delete world;
	}

	for (auto tex : textures)
	{
		delete tex;
	}

	for (auto light : lights)
	{
		delete light;
	}

	for (auto projector : projectors)
	{
		delete projector;
	}

	views.clear();
	textures.clear();
	lights.clear();

	delete[] tmeshes;
	tmeshes = nullptr;
}

void Scene::Render() 
{
	auto t1 = std::chrono::high_resolution_clock::now();
	for (auto world : views)
	{
		world->Render(*this, false);
	}
	auto t2 = std::chrono::high_resolution_clock::now();
	auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(t2 - t1).count();

	std::cout << "Time: "<< duration << std::endl;
}

void CopyIntoRender(Scene &scene)
{
	auto fbNoEffect = scene.views[0]->GetFB();
	auto fbAudience = scene.views[1]->GetFB();
	auto fbProjector = scene.projectors[0]->shadowMap->GetFB();
	auto fbRendered = scene.views[3]->GetFB();

	for (int i = 0; i < fbAudience->w; i++)
	{
		for (int j = 0; j < fbAudience->h; j++)
		{
			fbRendered->Set(i, j, fbAudience->Get(i, j));
		}
	}

	for (int i = 0; i < fbNoEffect->w; i++)
	{
		for (int j = 0; j < fbNoEffect->h; j++)
		{
			fbRendered->Set(i + fbAudience->w, j, fbNoEffect->Get(i, j));
		}
	}

	for (int i = 0; i < fbProjector->w; i++)
	{
		for (int j = 0; j < fbProjector->h; j++)
		{
			fbRendered->Set(i + fbNoEffect->w + fbAudience->w, j, fbProjector->Get(i, j));
		}
	}
}

void Scene::DBG() 
{
	FrameBuffer * fb = views[0]->GetFB();
	PPC * ppc = views[0]->GetPPC();

	{
		projectors[0]->UpdateShadowMap(*this);

		int stepsN = 300;
		char fname[30] = "";
		for (int i = 0; i < stepsN; i++)
		{
			projectors[0]->shadowMap->GetFB()->SetBGR(Vec3d::ZEROS.GetColor());
			tmeshes[0].Translate(Vec3d(0.75f, 0.0f, 0.0f));
			tmeshes[4].Rotate(tmeshes[4].GetCenter(), Vec3d::YAXIS, 360 / (float)stepsN);
			projectors[0]->UpdateShadowMap(*this);

			Render();
			CopyIntoRender(*this);
			sprintf_s(fname, "frames/frame_%d.tiff", i);
			views[3]->GetFB()->SaveAsTiff(fname);
			projectors[0]->shadowMap->Redraw();
			Fl::check();
		}
		return;
	}

	{
		PPC * ppc1 = new PPC(*ppc);
		PPC * ppc2 = new PPC(*ppc);
		Vec3d p2 = Vec3d(200, 150, 0);
		Vec3d p3 = Vec3d(0, 150, -200);
		ppc2->SetPose(p2, Vec3d::ZEROS, Vec3d::YAXIS);
		Light *light = scene->lights[0];

		projectors[0]->UpdateShadowMap(*this);
		light->UpdateShadowMap(*this);
		lights[1]->UpdateShadowMap(*this);

		//return;

		int stepsN = 360;
		for (int i = 0; i < stepsN/2; i++)
		{
			projectors[0]->UpdateShadowMap(*this);
			light->SetCenter(light->GetCenter().Rotate(Vec3d::ZEROS, Vec3d::YAXIS, -3.0f));
			light->UpdateShadowMap(*this);
			projectors[0]->SetCenter(projectors[0]->GetCenter().Rotate(Vec3d::ZEROS, Vec3d::YAXIS, -1.0f));
			ppc->SetPose(projectors[0]->GetCenter(), Vec3d::ZEROS, Vec3d::YAXIS);
			//ppc->Interpolate(ppc1, ppc2, i, stepsN/2);
			Render();
			Fl::check();
		}

		//ppc1->SetPose(p3, Vec3d::ZEROS, Vec3d::YAXIS);

		for (int i = 0; i < stepsN/2; i++)
		{
			light->SetCenter(light->GetCenter().Rotate(Vec3d::ZEROS, Vec3d::YAXIS, -3.0f));
			light->UpdateShadowMap(*this);
			projectors[0]->SetCenter(projectors[0]->GetCenter().Rotate(Vec3d::ZEROS, Vec3d::YAXIS, -1.0f));
			projectors[0]->UpdateShadowMap(*this);
			ppc->SetPose(projectors[0]->GetCenter(), Vec3d::ZEROS, Vec3d::YAXIS);
			//ppc->Interpolate(ppc2, ppc1, i, stepsN/2);
			Render();
			Fl::check();
		}


		return;
	}
}

TEX_HANDLE Scene::LoadTexture(const char * filename, bool isMipmap)
{
	Texture * buffer = new Texture(0, 0);
	if (!buffer->LoadTiff(filename))
	{
		delete buffer;
		return TEX_INVALID;
	}

	if (isMipmap)
	{
		buffer->SetAsMipmap(buffer->h);
	}
	else
	{
		buffer->mipH = buffer->h;
		buffer->mipW = buffer->w;
	}

	
	textures.push_back(buffer);
	return (TEX_HANDLE) textures.size() - 1;
}


void Scene::NewButton() 
{
	cerr << "INFO: pressed New Button" << endl;
}
