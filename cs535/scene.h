#pragma once

#include <vector>

#include "gui.h"
#include "TMesh.h"
#include "WorldView.h"
#include "Light.h"
#include "Projector.h"
#include "CubeMap.h"
#include "CGInterface.h"

class Vec3d;


class Scene {
public:

	GLuint defaultTexture;

	GUI *gui;

	std::unique_ptr<CGInterface> cgi;
	std::unique_ptr<SoftShadowsInterface> soi;

	std::vector<std::unique_ptr<WorldView>> views;
	std::vector<std::unique_ptr<FrameBuffer>> textures;
	std::vector<std::unique_ptr<TMesh>> tmeshes;

	bool hwPerSessionInit;

	Scene();
	~Scene();
	void DBG();
	void NewButton();
	void Render();
	void RenderHW();
	void RenderGPU();
	void Cleanup();
	void PerSessionHWInit();

	std::vector<std::unique_ptr<Light>> lights;
	std::vector<std::unique_ptr<Projector>> projectors;
	std::unique_ptr<CubeMap> cubeMap;
	std::unique_ptr<TMesh> areaLight;

	TEX_HANDLE LoadTexture(const char *, bool isMipmap);

	friend void WorldView::Render(Scene &scene, bool disableLighting);

	void RunReflections();
	void RunInvisibility();
	void RunShadows();
	void RunProjector();

};

extern Scene *scene;