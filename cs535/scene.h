#pragma once

#include <vector>

#include "gui.h"
#include "Texture.h"
#include "TMesh.h"
#include "WorldView.h"
#include "Light.h"
#include "Projector.h"

class Vec3d;


class Scene {
public:

	GUI *gui;
	std::vector<WorldView*> views;
	std::vector<Texture*> textures;
	std::vector<TMesh*> tmeshes;
	Scene();
	~Scene();
	void DBG();
	void NewButton();
	void Render();
	void Cleanup();

	std::vector<Light*> lights;
	std::vector<Projector*> projectors;

	TEX_HANDLE LoadTexture(const char *, bool isMipmap);

	friend void WorldView::Render(Scene &scene, bool disableLighting);

	void RunInvisibility();
	void RunShadows();
	void RunProjector();

};

extern Scene *scene;