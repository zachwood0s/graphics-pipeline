#include "stdafx.h"

#include "matrix3d.h"
#include "vec3d.h"
#include "scene.h"

Scene *scene;

using namespace std;

#include <iostream>

const double PI = 3.15159265;

Scene::Scene() {


	gui = new GUI();
	gui->show();

	int u0 = 20;
	int v0 = 20;
	int h = 480;
	int w = 640;

	fb = new FrameBuffer(u0, v0, w, h, 0);
	fb->label("SW frame buffer");
	fb->show();
	fb->redraw();

	gui->uiw->position(u0, v0 + h + 50);

	float hfov = 55.0f;
	ppc = new PPC(hfov, fb->w, fb->h);
	ppc->SetFocalLength(.5f);
	tmeshesN = 5;
	tmeshes = new TMesh[tmeshesN];

	Vec3d cc(0.0f, 0.0f, -100.0f);
	float sideLength = 60.0f;

	Render();


}

void CreateScene()
{

}

void Scene::Render() {

	fb->SetBGR(0xFFFFFF00);
	fb->ClearZB();


	for (int tmi = 0; tmi < tmeshesN; tmi++) {
		if (!tmeshes[tmi].onFlag)
			continue;
		tmeshes[tmi].DrawInterpolated(fb, ppc, 0xFF00FF00);
	}

	fb->redraw();


}

void Scene::DBG() {

	{
		Vec3d center = Vec3d::ZEROS;
		tmeshes[0].LoadBin("geometry/teapot57k.bin");
		tmeshes[0].SetCenter(center);
		tmeshes[0].ScaleTo(200);

		tmeshes[1].LoadBin("geometry/teapot1k.bin");
		tmeshes[1].SetCenter(Vec3d(-100.0f, -50.0f, 100.0f));
		tmeshes[1].ScaleTo(50);


		tmeshes[2].LoadBin("geometry/teapot1k.bin");
		tmeshes[2].SetCenter(Vec3d(-100.0f, -50.0f, -100.0f));
		tmeshes[2].ScaleTo(100);

		tmeshes[3].LoadBin("geometry/teapot1k.bin");
		tmeshes[3].SetCenter(Vec3d(100.0f, -50.0f, -100.0f));
		tmeshes[3].ScaleTo(50);

		tmeshes[4].LoadBin("geometry/teapot1k.bin");
		tmeshes[4].SetCenter(Vec3d(100.0f, -50.0f, 100.0f));
		tmeshes[4].ScaleTo(100);

		ppc->SetPose(Vec3d(100, 100, 200), center, Vec3d::YAXIS);

		Vec3d p1 = Vec3d(0, 100, 200);
		ppc->SetPose(p1, center, Vec3d::YAXIS);


		PPC * ppc1 = new PPC(*ppc);
		PPC * ppc2 = new PPC(*ppc);
		Vec3d p2 = Vec3d(-100, 175, -100);
		ppc2->SetPose(p2, center, Vec3d::YAXIS);

		int nSteps = 150;

		for (int i = 0; i < nSteps; i++)
		{
			Render();
			Fl::check();
			tmeshes[0].Rotate(center, Vec3d::YAXIS, 1.0f);
			tmeshes[1].Rotate(tmeshes[1].GetCenter(), Vec3d(1,1,1), 2.0f);
			tmeshes[2].Rotate(tmeshes[2].GetCenter(), Vec3d(1,1,1), 3.0f);
			tmeshes[3].Rotate(tmeshes[3].GetCenter(), Vec3d(1,1,1), 5.0f);
			tmeshes[4].Rotate(tmeshes[4].GetCenter(), Vec3d(1,1,1), 5.0f);
		}

		for (int i = 0; i < nSteps; i++)
		{
			Render();
			Fl::check();
			tmeshes[0].Rotate(center, Vec3d::YAXIS, 1.0f);
			tmeshes[1].Rotate(tmeshes[1].GetCenter(), Vec3d(1,1,1), 2.0f);
			tmeshes[2].Rotate(tmeshes[2].GetCenter(), Vec3d(1,1,1), 3.0f);
			tmeshes[3].Rotate(tmeshes[3].GetCenter(), Vec3d(1,1,1), 4.0f);
			tmeshes[4].Rotate(tmeshes[4].GetCenter(), Vec3d(1,1,1), 5.0f);
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
			//ppc->PanLeftRight(1);
			//ppc->TiltUpDown(1);
			tmeshes[1].Rotate(tmeshes[1].GetCenter(), Vec3d::YAXIS, 1.0f);
			ppc->SetFocalLength(1.001f);
		}
		return;
	}

	{
		cerr << "INFO: Running animation" << endl;
		Vec3d origin = Vec3d(300, 300, 0);
		Vec3d point = Vec3d(100, 200, 0);
		Vec3d axis = Vec3d(2, 2, 2).Normalized();
		cout << "AXIS: " << axis << endl;
		cout << "ORIGIN: " << origin << endl;
		cout << "POINT START: " << point << endl;

		int r = 5;
		float deg_step = PI / 180;
		for (int i = 0; i <= 360; i++)
		{
			fb->SetBGR(0xFF00FFFF);
			fb->DrawCircle((int)point[0], (int)point[1], r, 0xFFFF0000);
			//fb->SetCircle((int)origin[0], (int)origin[1], r * 2, 0xFFFF0000);
			fb->redraw();
			point = point.Rotate(origin, axis, deg_step);
			Fl::check();
		}

	}
}


void Scene::NewButton() {
	cerr << "INFO: pressed New Button" << endl;
}
