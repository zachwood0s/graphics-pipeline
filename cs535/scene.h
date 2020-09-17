#pragma once

#include <vector>

#include "gui.h"
#include "framebuffer.h"
#include "TMesh.h"
#include "WorldView.h"


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

	friend void WorldView::Render(Scene &scene);

};

extern Scene *scene;