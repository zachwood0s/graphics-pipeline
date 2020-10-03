#include "stdafx.h"

#include "matrix3d.h"
#include "vec3d.h"
#include "scene.h"

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

	WorldView * world = new WorldView("SW Frame Buffer", u0, v0, w, h, hfov, (int) views.size());
	//world->GetPPC()->ZoomFocalLength(.5f);
	world->showCameraBox = false;
	world->showCameraScreen = false;
	world->background.SetFromColor(0xFFFFFFFF);

	views.push_back(world);

//	world = new WorldView("SW 3", u0 + w + 20, v0, w, h, hfov, (int) views.size());
//	world->GetPPC()->ZoomFocalLength(.5f);
//	world->cameraVf = 100.0f;

//	views.push_back(world);

	gui->uiw->position(u0, v0 + h + 50);

	tmeshesN = 2;
	tmeshes = new TMesh[tmeshesN];

	TEX_HANDLE head = LoadTexture("textures/fox_head_color.tiff", false);
	TEX_HANDLE checker = LoadTexture("textures/checker.tiff", false);
	TEX_HANDLE mountains = LoadTexture("textures/mountains.tiff", true);

	//tmeshes[0].LoadBin("geometry/happy4.bin");
	tmeshes[0].LoadObj("geometry/fox.obj");
	tmeshes[0].ScaleTo(200);
	tmeshes[0].SetCenter(Vec3d::ZEROS);
	tmeshes[0].SetMaterial({ Vec3d(.2f, .2f, .4f), 32, 0.8f, head});
	tmeshes[0].onFlag = false;

	tmeshes[1].SetToPlane(Vec3d::ZEROS, 2000, 2000);
	tmeshes[1].SetMaterial({ Vec3d::ZEROS, 32, 0.5f, mountains });
	tmeshes[1].Rotate(Vec3d::ZEROS, Vec3d::XAXIS, -90.0f);
	tmeshes[1].Translate(Vec3d(0.0f, -90.0f, -500.0f));
	for (int i = 0; i < tmeshes[1].texsN; i++)
	{
		tmeshes[1].texs[i] = tmeshes[1].texs[i] * 2;
	}

	//tmeshes[0].SetToCube(Vec3d::ZEROS, 100, 0xff00ff00, 0xff0000ff);
	//tmeshes[0].Rotate(Vec3d::ZEROS, Vec3d::YAXIS, 90.0f);

	views[0]->GetPPC()->SetPose(Vec3d(0, 50, 200), Vec3d::ZEROS, Vec3d::YAXIS);
	light = Vec3d(world->GetPPC()->C);
//	views[1]->GetPPC()->SetPose(Vec3d(200, 0, -300), Vec3d::ZEROS, Vec3d::YAXIS);


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

	views.clear();
	textures.clear();

	delete[] tmeshes;
	tmeshes = nullptr;
}

void Scene::Render() 
{
	for (auto world : views)
	{
		world->Render(*this);
	}
}

void Scene::DBG() 
{

	PPC * ppc = views[0]->GetPPC();

	{
		for (int i = 0; i < 720; i++)
		{
			light = light.Rotate(Vec3d::ZEROS, Vec3d::YAXIS, 1);
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
		buffer->SetAsMipmap((2 * buffer->w) / 3, buffer->h);
	
	textures.push_back(buffer);
	return (TEX_HANDLE) textures.size() - 1;
}


void Scene::NewButton() 
{
	cerr << "INFO: pressed New Button" << endl;
}
