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

	Render();
}

Scene::~Scene()
{
	Cleanup();
}

void Scene::Cleanup()
{
	views.clear();
	textures.clear();
	lights.clear();
	tmeshes.clear();
	lights.clear();
	projectors.clear();
}

void Scene::Render() 
{
	auto t1 = std::chrono::high_resolution_clock::now();
	for (auto& world : views)
	{
		world->Render(*this, false);
	}
	auto t2 = std::chrono::high_resolution_clock::now();
	auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(t2 - t1).count();

	std::cout << "Time: "<< duration << std::endl;
}

void Scene::RenderHW()
{
	auto t1 = std::chrono::high_resolution_clock::now();
	for (auto& world : views)
	{
		world->RenderHW(*this);
	}
	auto t2 = std::chrono::high_resolution_clock::now();
	auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(t2 - t1).count();

	std::cout << "Time: "<< duration << std::endl;
}

void Scene::RenderGPU()
{
	auto t1 = std::chrono::high_resolution_clock::now();
	for (auto& world : views)
	{
		world->RenderGPU(*this);
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

void Scene::RunReflections() 
{
	Cleanup();

	int u0 = 20;
	int v0 = 20;
	int h = 480;
	int w = 640;
	float hfov = 55.0f;
	Vec3d cameraStart(0.0f, 0.0f, 30.0f);

	scene->cubeMap = CubeMap::FromSingleTiff("textures/uffizi_cross.tiff", true);

	auto world = std::make_unique<WorldView>("Simple Reflections demo", u0, v0 + h + 20, w, h, hfov, (int) views.size());
	world->background.SetFromColor(0xFFFFFFFF);
	world->kAmbient = .05f;
	world->GetPPC()->ZoomFocalLength(.5f);
	world->GetPPC()->SetPose(cameraStart, Vec3d::ZEROS, Vec3d::YAXIS);
	world->shaders.push_back(Shaders::reflectionShader);
	world->backgroundShader = Shaders::environmentMap;
	views.push_back(std::move(world));

	TEX_HANDLE checker = LoadTexture("textures/checker.tiff", false);
	TEX_HANDLE head = LoadTexture("textures/fox_head_color.tiff", false);

	// Main Teapot
	auto mesh = std::make_unique<TMesh>();
	mesh->LoadObj("geometry/teapot.obj");
	mesh->ScaleTo(50);
	mesh->SetCenter(Vec3d::ZEROS);
	mesh->SetMaterial({ Vec3d::ONES * .99f, 32, 0.9f, (GLuint) TEX_INVALID });
	mesh->GetMaterial().reflectivity = 1.0f;
	mesh->onFlag = true;
	tmeshes.push_back(std::move(mesh));

	{
		int framesN = 300;
		char fname[40] = "";
		auto ppc = views[0]->GetPPC();
		Vec3d center = ppc->C;
		
		for (int i = 0; i < framesN; i++)
		{
			center = center.Rotate(Vec3d::ZEROS, Vec3d::YAXIS, 180.0f / framesN);
			ppc->SetPose(center, Vec3d::ZEROS, Vec3d::YAXIS);

			//sprintf_s(fname, "frames/reflections/frame_%d.tiff", i);
			//views[0]->GetFB()->SaveAsTiff(fname);
			Render();
			Fl::check();
		}
		for (int i = 0; i < framesN; i++)
		{
			center = center.Rotate(Vec3d::ZEROS, Vec3d::XAXIS, 180.0f / framesN);
			center = center.Rotate(Vec3d::ZEROS, Vec3d::YAXIS, 3.0f / framesN); // To prevent jarring flip at the top
			ppc->SetPose(center, Vec3d::ZEROS, Vec3d::YAXIS);

			//sprintf_s(fname, "frames/reflections/frame_%d.tiff", i + framesN);
			//views[0]->GetFB()->SaveAsTiff(fname);
			Render();
			Fl::check();
		}
	}
}

void Scene::RunShadows() 
{
	Cleanup();

	int u0 = 20;
	int v0 = 20;
	int h = 480;
	int w = 640;
	float hfov = 55.0f;
	Vec3d cameraStart(20.0f, 30.0f, 30.0f);


	//scene->cubeMap = CubeMap::FromSingleTiff("textures/uffizi_cross.tiff", true);

	// Create the stationary camera world view
	auto world = std::make_unique<WorldView>("Simple Shadow Demo", u0, v0 + h + 20, w, h, hfov, (int) views.size());
	world->background.SetFromColor(0xFFFF0000);
	world->SetMode(WorldView::GPU);
	//world->showCameraBox = true;
	//world->showCameraScreen = true;
	world->kAmbient = .05f;
	world->GetPPC()->ZoomFocalLength(.5f);
	world->GetPPC()->SetPose(cameraStart, Vec3d::ZEROS, Vec3d::YAXIS);
	views.push_back(std::move(world));

	// Bottom plane
	auto mesh = std::make_unique<TMesh>();
	mesh->LoadObj("geometry/plane.obj");
	mesh->ScaleTo(200);
	mesh->SetCenter(Vec3d(0.0f, 0.0f, 0.0f));
	mesh->SetMaterial({ Vec3d::ONES * .9f, 32, 0.9f, (GLuint) TEX_INVALID });
	mesh->GetMaterial().reflectivity = 0.7f;
	tmeshes.push_back(std::move(mesh));

	mesh = std::make_unique<TMesh>();
	mesh->LoadObj("geometry/cube.obj");
	mesh->ScaleTo(10);
	mesh->SetCenter(Vec3d(0.0f, 3.0f, 0.0f));
	mesh->Rotate(mesh->GetCenter(), Vec3d::ZAXIS, 0.0f);
	mesh->SetMaterial({ Vec3d::XAXIS, 32, 0.9f, (GLuint) TEX_INVALID });
	mesh->GetMaterial().reflectivity = 0.7f;
	tmeshes.push_back(std::move(mesh));

	mesh = std::make_unique<TMesh>();
	mesh->LoadObj("geometry/cube.obj");
	mesh->ScaleTo(10);
	mesh->SetCenter(Vec3d(10.0f, 3.0f, 0.0f));
	mesh->Rotate(mesh->GetCenter(), Vec3d::ZAXIS, 0.0f);
	mesh->SetMaterial({ Vec3d::YAXIS, 32, 0.9f, (GLuint) TEX_INVALID });
	mesh->GetMaterial().reflectivity = 0.7f;
	tmeshes.push_back(std::move(mesh));

	mesh = std::make_unique<TMesh>();
	mesh->LoadObj("geometry/cube.obj");
	mesh->ScaleTo(10);
	mesh->SetCenter(Vec3d(-10.0f, 3.0f, 0.0f));
	mesh->Rotate(mesh->GetCenter(), Vec3d::ZAXIS, 0.0f);
	mesh->SetMaterial({ Vec3d::ZAXIS, 32, 0.9f, (GLuint) TEX_INVALID });
	mesh->GetMaterial().reflectivity = 0.7f;
	tmeshes.push_back(std::move(mesh));

	// Light rectangle
	areaLight = std::make_unique<TMesh>();
	areaLight->LoadObj("geometry/plane.obj");
	float scale = 5.0f;
	areaLight->ScaleTo(scale);
	areaLight->SetCenter(Vec3d(-10.0f, 20.0f, 5.0f));


	{
		int framesN = 600;
		char fname[50] = "";
		for (int i = 0; i < framesN; i++)
		{
			tmeshes[1]->Rotate(Vec3d::ZEROS, Vec3d::YAXIS, 360 / (float) framesN);
			tmeshes[2]->Rotate(Vec3d::ZEROS, Vec3d::YAXIS, 360 / (float) framesN);
			tmeshes[3]->Rotate(Vec3d::ZEROS, Vec3d::YAXIS, 360 / (float) framesN);
			//areaLight->Rotate(Vec3d::ZEROS, Vec3d::YAXIS, 1);
			//views[0]->GetPPC()->SetPose(views[0]->GetPPC()->C.Rotate(Vec3d::ZEROS, Vec3d::YAXIS, 1), Vec3d::ZEROS, Vec3d::YAXIS);

			//Render();

			views[0]->Redraw();

			Fl::check();
			//glReadPixels(0, 0, views[0]->GetFB()->w, views[0]->GetFB()->h, GL_RGBA, GL_UNSIGNED_BYTE, views[0]->GetFB()->pix);
			sprintf_s(fname, "frames/soft_shadows/frame_%d.tiff", i);
			views[0]->GetFB()->SaveAsTiff(fname);
		}
		for (int i = 0; i < framesN; i++)
		{
			areaLight->Translate((Vec3d::XAXIS * 20.0f) / framesN);
			views[0]->Redraw();
			Fl::check();
		}
		for (int i = 0; i < framesN; i++)
		{
			areaLight->ScaleTo(scale + 0.1*i);
			views[0]->Redraw();
			Fl::check();
		}
	}
}

void Scene::RunProjector() 
{
	Cleanup();

	int u0 = 20;
	int v0 = 20;
	int h = 480;
	int w = 640;
	float hfov = 55.0f;
	Vec3d cameraStart(-3.0f, 3.0f, 3.0f);

	TEX_HANDLE hello = LoadTexture("textures/hellotext.tiff", false);

	// Create the stationary camera world view
	auto world = std::make_unique<WorldView>("Projector Demo", u0, v0 + h + 20, w, h, hfov, (int) views.size());
	world->background.SetFromColor(0xFFFFFFFF);
	world->kAmbient = .05f;
	world->GetPPC()->ZoomFocalLength(.5f);
	world->GetPPC()->SetPose(cameraStart, Vec3d::ZEROS, Vec3d::YAXIS);
	world->shaders.push_back(Shaders::phongShading);
	world->shaders.push_back(Shaders::projectiveTextures);
	views.push_back(std::move(world));

	auto mesh = std::make_unique<TMesh>();
	mesh->LoadObj("geometry/oceanScene.obj");
	mesh->ScaleTo(10);
	mesh->SetCenter(Vec3d::ZEROS);
	mesh->onFlag = true;
	mesh->SetMaterial({ Vec3d::ONES * .8f, 32, 0.9f, (GLuint) TEX_INVALID });
	tmeshes.push_back(std::move(mesh));

	Vec3d lightStart = Vec3d(2.5f, 1.5f, 2.0f);
	auto light = std::make_unique<Light>(lightStart, 1024, 1024, 90.0f);
	lights.push_back(std::move(light));

	auto projector = std::make_unique<Projector>(lightStart, w, h, 90.0f, hello);
	projectors.push_back(std::move(projector));



	{
		int framesN = 300;
		char fname[50] = "";
		for (int i = 0; i < framesN; i++)
		{
			Vec3d n = projectors[0]->GetCenter().Rotate(Vec3d::ZEROS, Vec3d::YAXIS, 360.0 / framesN);
			projectors[0]->SetCenter(n);
			for (auto& light : lights) light->UpdateShadowMap(*this);
			for (auto& projector : projectors) projector->UpdateShadowMap(*this);
			//sprintf_s(fname, "frames/projector/frame_%d.tiff", i);
			//views[0]->GetFB()->SaveAsTiff(fname);
			Render();
			Fl::check();
		}
	}

	Render();

}

void Scene::RunInvisibility()
{
	Cleanup();

	int u0 = 20;
	int v0 = 20;
	int h = 480;
	int w = 640;
	float hfov = 55.0f;

	Vec3d cameraStart(0.0f, 50.0f, 200.0f);

	// Create the stationary camera world view
	auto world = std::make_unique<WorldView>("Audience Without the Effect", u0, v0 + h + 20, w, h, hfov, (int) views.size());
	world->background.SetFromColor(0xFFFFFFFF);
	world->kAmbient = .05f;
	world->GetPPC()->ZoomFocalLength(.5f);
	world->GetPPC()->SetPose(cameraStart, Vec3d::ZEROS, Vec3d::YAXIS);
	world->shaders.push_back(Shaders::invisibilityShader);
	views.push_back(std::move(world));

	// Create the moving camera world view
	world = std::make_unique<WorldView>("Audience", u0, v0, w, h, hfov, (int) views.size());
	world->background.SetFromColor(0xFFFFFFFF);
	world->GetPPC()->ZoomFocalLength(.5f);
	world->kAmbient = .05f;
	world->GetPPC()->SetPose(cameraStart, Vec3d::ZEROS, Vec3d::YAXIS);
	world->shaders.push_back(Shaders::projectiveTextures);
	views.push_back(std::move(world));

	// This won't be shown as a screen
	world = std::make_unique<WorldView>(w, h, hfov, (int) views.size());
	world->background.SetFromColor(0xFFFFFFFF);
	world->kAmbient = .05f;
	world->GetPPC()->ZoomFocalLength(.5f);
	world->GetPPC()->SetPose(cameraStart, Vec3d::ZEROS, Vec3d::YAXIS);
	world->hiddenMeshes.insert(0);
	views.push_back(std::move(world));

	// Create the side-by-side render world
	world = std::make_unique<WorldView>(w*3, h, hfov, (int) views.size());
	world->background.SetFromColor(0xFFFFFFFF);
	world->kAmbient = .05f;
	world->GetPPC()->ZoomFocalLength(.5f);
	world->GetPPC()->SetPose(cameraStart, Vec3d::ZEROS, Vec3d::YAXIS);
	views.push_back(std::move(world));

	gui->uiw->position(u0 + w, v0 + h + 50);

	TEX_HANDLE head = LoadTexture("textures/fox_head_color.tiff", false);
	TEX_HANDLE checker = LoadTexture("textures/checker.tiff", false);
	TEX_HANDLE mountains = LoadTexture("textures/mountains.tiff", true);
	TEX_HANDLE hole = LoadTexture("textures/hole.tiff", false);
	TEX_HANDLE crate = LoadTexture("textures/crate.tiff", false);
	TEX_HANDLE mirror = LoadTexture("textures/mirror.tiff", false);
	TEX_HANDLE hello = LoadTexture("textures/hellotext.tiff", false);

	// left cube
	auto mesh = std::make_unique<TMesh>();
	mesh->LoadObj("geometry/cube.obj");
	mesh->ScaleTo(60);
	mesh->SetCenter(Vec3d(-80, 30, 40));
	mesh->SetMaterial({ Vec3d::ONES * .1f, 32, 0.9f, checker });
	mesh->onFlag = true;
	tmeshes.push_back(std::move(mesh));

	// Bottom plane
	mesh = std::make_unique<TMesh>();
	mesh->LoadObj("geometry/plane.obj");
	mesh->ScaleTo(600);
	mesh->SetCenter(Vec3d(0.0f, 0.0f, -70.0f));
	mesh->SetMaterial({ Vec3d::ONES * .1f, 32, 0.9f, checker });
	mesh->onFlag = true;
	tmeshes.push_back(std::move(mesh));

	// Back plane
	mesh = std::make_unique<TMesh>();
	mesh->LoadObj("geometry/plane.obj");
	mesh->ScaleTo(600);
	mesh->Rotate(Vec3d::ZEROS, Vec3d::XAXIS, 90.0f);
	mesh->SetCenter(Vec3d(0.0f, 0.0f, -40.0f));
	mesh->SetMaterial({ Vec3d::YAXIS * .1f, 8, 0.5f, mountains });
	mesh->onFlag = true;
	tmeshes.push_back(std::move(mesh));

	// Fox
	mesh = std::make_unique<TMesh>();
	mesh->LoadObj("geometry/fox.obj");
	mesh->ScaleTo(100);
	mesh->SetCenter(Vec3d(0.0f, 30.0f, -0.0f));
	mesh->SetMaterial({ Vec3d::ONES * .1f, 32, 0.9f, head });
	mesh->onFlag = true;
	tmeshes.push_back(std::move(mesh));

	// Cube with hole
	mesh = std::make_unique<TMesh>();
	mesh->LoadObj("geometry/cube.obj");
	mesh->ScaleTo(110);
	mesh->SetCenter(Vec3d(-5.0f, 30, -0));
	mesh->SetMaterial({ Vec3d::ONES * .1f, 32, 0.1f, hole });
	mesh->onFlag = true;
	tmeshes.push_back(std::move(mesh));

	// Create the projector
	auto projector = std::make_unique<Projector>(Vec3d(50, 100, 230), w*2, h*2, 90.0f, TEX_INVALID);
	projector->shadowMap->shaders.pop_back();
	projectors.push_back(std::move(projector));

	Render();

	// Run the simulation
	{
		projectors[0]->UpdateShadowMap(*this);

		int stepsN = 300;
		char fname[30] = "";
		for (int i = 0; i < stepsN; i++)
		{
			projectors[0]->shadowMap->GetFB()->SetBGR(Vec3d::ZEROS.GetColor());
			tmeshes[0]->Translate(Vec3d(0.75f, 0.0f, 0.0f));
			tmeshes[4]->Rotate(tmeshes[4]->GetCenter(), Vec3d::YAXIS, 360 / (float)stepsN);
			projectors[0]->UpdateShadowMap(*this);

			Render();
			CopyIntoRender(*this);
			//sprintf_s(fname, "frames/frame_%d.tiff", i);
			//views[3]->GetFB()->SaveAsTiff(fname);
			projectors[0]->shadowMap->Redraw();
			Fl::check();
		}
		return;
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
			tmeshes[0]->Translate(Vec3d(0.75f, 0.0f, 0.0f));
			tmeshes[4]->Rotate(tmeshes[4]->GetCenter(), Vec3d::YAXIS, 360 / (float)stepsN);
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
		auto& light = scene->lights[0];

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
	auto buffer = std::make_unique<FrameBuffer>(0, 0);
	if (!buffer->LoadTiff(filename))
	{
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

	
	textures.push_back(std::move(buffer));
	return (TEX_HANDLE) textures.size() - 1;
}

GLubyte texData[] = { 255, 255, 255, 255 };

void Scene::PerSessionHWInit() {

	if (hwPerSessionInit)
		return;

	// create textures
	// 1. Create texture handles "glCreateTextures"
	// 2. Create each texture for each texture handle "glTexImage2D"
	//		send the texels to be used for each texture to HW
	//		the texels can be sent from a FrameBuffer object, i.e. the unsigned int *pix pointer

	glGenTextures(1, &defaultTexture);
	glBindTexture(GL_TEXTURE_2D, defaultTexture);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 1, 1, 0, GL_RGBA, GL_UNSIGNED_BYTE, texData);


	TEX_HANDLE checker = LoadTexture("textures/fox_head_color.tiff", false);

	if (checker != TEX_INVALID)
	{
		GLuint textureHandle = 0;
		glGenTextures(1, &textureHandle);
		glBindTexture(GL_TEXTURE_2D, textureHandle);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, textures[checker]->w, textures[checker]->h, 0, GL_RGBA, GL_UNSIGNED_BYTE, textures[checker]->pix);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
		//tmeshes[4]->GetMaterial().texture = textureHandle;
	}

	//tex_handles.push_back(texName);
	hwPerSessionInit = true;

}


void Scene::NewButton() 
{
	cerr << "INFO: pressed New Button" << endl;
}
