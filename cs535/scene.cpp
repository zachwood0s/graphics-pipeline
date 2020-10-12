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

	Vec3d cameraStart(0.0f, 150.0f, 200.0f);

	// Create the moving camera world view
	WorldView * world = new WorldView("Moving Camera", u0, v0, w, h, hfov, (int) views.size());
	world->background.SetFromColor(0xFFFFFFFF);
	world->kAmbient = .05f;
	world->GetPPC()->SetPose(cameraStart, Vec3d::ZEROS, Vec3d::YAXIS);

	world->shaders.push_back(Shaders::phongShading);
	world->shaders.push_back(Shaders::projectiveTextures);

	views.push_back(world);

	// Create the stationary camera world view
	world = new WorldView("Stationary Camera", u0 + w + 20, v0, w, h, hfov, (int) views.size());
	world->showCameraBox = true;
	world->background.SetFromColor(0xFFFFFFFF);
	world->kAmbient = .05f;
	world->GetPPC()->ZoomFocalLength(.5f);
	world->GetPPC()->SetPose(cameraStart, Vec3d::ZEROS, Vec3d::YAXIS);

	world->shaders.push_back(Shaders::phongShading);
	world->shaders.push_back(Shaders::projectiveTextures);

	views.push_back(world);

	gui->uiw->position(u0, v0 + h + 50);

	tmeshesN = 6;
	tmeshes = new TMesh[tmeshesN];

	TEX_HANDLE head = LoadTexture("textures/fox_head_color.tiff", false);
	TEX_HANDLE checker = LoadTexture("textures/checker.tiff", false);
	TEX_HANDLE mountains = LoadTexture("textures/mountains.tiff", true);
	TEX_HANDLE hole = LoadTexture("textures/hole.tiff", false);
	TEX_HANDLE crate = LoadTexture("textures/crate.tiff", false);
	TEX_HANDLE mirror = LoadTexture("textures/mirror.tiff", false);
	TEX_HANDLE hello = LoadTexture("textures/hellotext.tiff", false);

	// Fox
	tmeshes[0].LoadObj("geometry/fox.obj");
	tmeshes[0].ScaleTo(100);
	tmeshes[0].SetCenter(Vec3d(3.0f, 0, 0));
	tmeshes[0].SetMaterial({ Vec3d::ONES * .1f, 32, 0.8f, TEX_INVALID});
	tmeshes[0].onFlag = true;

	// Cube
	tmeshes[1].LoadObj("geometry/cube.obj");
	tmeshes[1].ScaleTo(120);
	tmeshes[1].SetCenter(Vec3d::ZEROS);
	tmeshes[1].SetMaterial({ Vec3d::ONES * .1f, 32, 0.1f, TEX_INVALID });
	tmeshes[1].onFlag = true;

	// left cube
	tmeshes[2].LoadObj("geometry/cube.obj");
	tmeshes[2].ScaleTo(60);
	tmeshes[2].SetCenter(Vec3d(-80, 0, 0));
	tmeshes[2].SetMaterial({ Vec3d::ONES * .1f, 32, 0.1f, TEX_INVALID });
	tmeshes[2].onFlag = true;

	// Bottom plane
	tmeshes[3].LoadObj("geometry/cube.obj");
	tmeshes[3].ScaleTo(60);
	tmeshes[3].SetCenter(Vec3d(80, 0, 0));
	tmeshes[3].SetMaterial({ Vec3d::ONES * .1f, 32, 0.9f, TEX_INVALID });
	tmeshes[3].onFlag = false;

	// Bottom plane
	tmeshes[4].LoadObj("geometry/plane.obj");
	tmeshes[4].ScaleTo(600);
	tmeshes[4].SetCenter(Vec3d(0, -40.0f, 0.0f));
	tmeshes[4].SetMaterial({ Vec3d::ONES * .1f, 8, 0.5f, TEX_INVALID });
	tmeshes[4].onFlag = true;

	

	Light * light = new Light(Vec3d(-140, 254, 140), 500, 500, 90.0f);
	light->SetAttenuation(1.0f, 0.00000045f, 0.000000075f);
	lights.push_back(light);

	light = new Light(Vec3d(-140, 104, 140), 500, 500, 90.0f);
	light->SetAttenuation(1.0f, 0.00000045f, 0.000000075f);
	lights.push_back(light);


	Projector * projector = new Projector(Vec3d(0, 150, 200), 512, 512, 50.0f, hello);
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

void Scene::DBG() 
{
	FrameBuffer * fb = views[0]->GetFB();
	PPC * ppc = views[0]->GetPPC();
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
			light->SetCenter(light->GetCenter().Rotate(Vec3d::ZEROS, Vec3d::YAXIS, -3.0f));
			light->UpdateShadowMap(*this);
			projectors[0]->SetCenter(projectors[0]->GetCenter().Rotate(Vec3d::ZEROS, Vec3d::YAXIS, -1.0f));
			projectors[0]->UpdateShadowMap(*this);
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


	{
		Vec3d center = Vec3d::ZEROS;

		tmeshes[1].LoadBin("geometry/teapot1k.bin");
		tmeshes[1].SetCenter(Vec3d(-100.0f, -50.0f, 100.0f));
		tmeshes[1].ScaleTo(50);


		tmeshes[2].LoadBin("geometry/happy4.bin");
		tmeshes[2].SetCenter(Vec3d(-100.0f, -50.0f, -100.0f));
		tmeshes[2].ScaleTo(100);

		tmeshes[3].LoadBin("geometry/teapot1k.bin");
		tmeshes[3].SetCenter(Vec3d(100.0f, -50.0f, -100.0f));
		tmeshes[3].ScaleTo(50);

		tmeshes[4].LoadBin("geometry/happy4.bin");
		tmeshes[4].SetCenter(Vec3d(100.0f, -50.0f, 100.0f));
		tmeshes[4].ScaleTo(100);

		//views[1]->GetPPC()->SetPose(Vec3d(100, 100, 300), center, Vec3d::YAXIS);

		Vec3d p1 = Vec3d(0, 100, 200);
		ppc->SetPose(p1, center, Vec3d::YAXIS);


		PPC * ppc1 = new PPC(*ppc);
		PPC * ppc2 = new PPC(*ppc);
		Vec3d p2 = Vec3d(-100, 175, -100);
		ppc2->SetPose(p2, center, Vec3d::YAXIS);
		Render();

		int nSteps = 150;

		for (int i = 0; i < nSteps; i++)
		{
			Render();
			Fl::check();
			tmeshes[0].Rotate(center, Vec3d::YAXIS, 3.0f);
			tmeshes[1].Rotate(center, Vec3d::YAXIS, 2.0f);
			tmeshes[2].Rotate(center, Vec3d::ZAXIS, 3.0f);
			tmeshes[3].Rotate(center, Vec3d::XAXIS, 4.0f);
			tmeshes[4].Rotate(center, Vec3d(1,1,1).Normalized(), 5.0f);
		}

		for (int i = 0; i < nSteps; i++)
		{
			Render();
			Fl::check();
			tmeshes[0].Rotate(center, Vec3d::YAXIS, 3.0f);
			tmeshes[1].Rotate(center, Vec3d::YAXIS, 2.0f);
			tmeshes[2].Rotate(center, Vec3d::ZAXIS, 3.0f);
			tmeshes[3].Rotate(center, Vec3d::XAXIS, 4.0f);
			tmeshes[4].Rotate(center, Vec3d(1,1,1).Normalized(), 5.0f);
			ppc->Interpolate(ppc1, ppc2, i, nSteps);
		}

		return;
	}

	{
		Vec3d tcenter = tmeshes[1].GetCenter();

		/*
		Vec3d smallest;
		Vec3d largest;
		tmeshes[1].GetAABB(smallest, largest);
		cout << smallest << " <> " << largest << endl;
		cout << (largest - smallest).Length()  << endl;

		tmeshes[1].ScaleTo(20);
		Render();
		tmeshes[1].GetAABB(smallest, largest);
		cout << (largest - smallest).Length() << endl;
			*/
		Vec3d aDir(0.0f, 1.0f, 0.0f);
		for (int i = 0; i < 360; i++)
		{
			Render();
			Fl::check();
			Vec3d newC = Vec3d(0.0f, 0.0f, 0.0f - i);
			//ppc->TranslateFrontBack(-1);
			//ppc->TranslateRightLeft(-1);
			//ppc->TranslateUpDown(-1);
			//ppc->Roll(1);
			ppc->PanLeftRight(1);
			//ppc->TiltUpDown(1);
			//tmeshes[1].Rotate(tmeshes[1].GetCenter(), Vec3d::YAXIS, 1.0f);
			//ppc->SetFocalLength(1.001f);
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
