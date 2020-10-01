#pragma once

#include <vector>

#include "gui.h"
#include "framebuffer.h"
#include "TMesh.h"
#include "WorldView.h"

class Vec3d;

using TEX_HANDLE = int;

constexpr auto TEX_INVALID = -1;

class Scene {
public:

	GUI *gui;
	std::vector<WorldView*> views;
	std::vector<FrameBuffer*> textures;
	TMesh *tmeshes;
	int tmeshesN;
	Scene();
	~Scene();
	void DBG();
	void NewButton();
	void Render();

	Vec3d light;

	TEX_HANDLE LoadTexture(const char *);

	friend void WorldView::Render(Scene &scene);

};

extern Scene *scene;