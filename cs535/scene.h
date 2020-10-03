#pragma once

#include <vector>

#include "gui.h"
#include "Texture.h"
#include "TMesh.h"
#include "WorldView.h"

class Vec3d;


class Scene {
public:

	GUI *gui;
	std::vector<WorldView*> views;
	std::vector<Texture*> textures;
	TMesh *tmeshes;
	int tmeshesN;
	Scene();
	~Scene();
	void DBG();
	void NewButton();
	void Render();

	Vec3d light;

	TEX_HANDLE LoadTexture(const char *, bool isMipmap);

	friend void WorldView::Render(Scene &scene);

};

extern Scene *scene;