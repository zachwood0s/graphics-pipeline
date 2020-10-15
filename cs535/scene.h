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
	std::vector<std::unique_ptr<WorldView>> views;
	std::vector<std::unique_ptr<Texture>> textures;
	std::vector<std::unique_ptr<TMesh>> tmeshes;
	Scene();
	~Scene();
	void DBG();
	void NewButton();
	void Render();
	void Cleanup();

	std::vector<std::unique_ptr<Light>> lights;
	std::vector<std::unique_ptr<Projector>> projectors;

	TEX_HANDLE LoadTexture(const char *, bool isMipmap);

	friend void WorldView::Render(Scene &scene, bool disableLighting);

	void RunInvisibility();
	void RunShadows();
	void RunProjector();

};

extern Scene *scene;