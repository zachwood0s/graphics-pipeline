#pragma once

#include <vector>

#include "gui.h"
#include "framebuffer.h"
#include "TMesh.h"
#include "WorldView.h"

class Vec3d;

class Scene {
public:

	GUI *gui;
	std::vector<WorldView*> views;
	TMesh *tmeshes;
	int tmeshesN;
	Scene();
	~Scene();
	void DBG();
	void NewButton();
	void Render();

	Vec3d light;

	friend void WorldView::Render(Scene &scene);

};

extern Scene *scene;