#pragma once

#include "gui.h"
#include "framebuffer.h"
#include "TMesh.h"


class Scene {
public:

	GUI *gui;
	FrameBuffer *fb;
	PPC *ppc;
	TMesh *tmeshes;
	int tmeshesN;
	Scene();
	void DBG();
	void NewButton();
	void Render();
};

extern Scene *scene;