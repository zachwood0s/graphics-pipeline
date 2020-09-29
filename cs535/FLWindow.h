#pragma once

#include <FL/Fl.H>
#include <FL/Fl_Gl_Window.H>
#include <GL/glut.h>

#include "framebuffer.h"

class FLWindow : public Fl_Gl_Window
{
private:
	FrameBuffer* buffer;

public:
	FLWindow(int u0, int v0, FrameBuffer* buffer, const char* label);

	void draw();
	void KeyboardHandle();
	int handle(int guievent);

};

