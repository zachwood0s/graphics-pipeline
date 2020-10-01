#include "FLWindow.h"
#include "scene.h"

using namespace std;

FLWindow::FLWindow(int u0, int v0, FrameBuffer* _buffer, const char* label)
	: Fl_Gl_Window(u0, v0, _buffer->w, _buffer->h, label)
{
	buffer = _buffer;
}

void FLWindow::draw() 
{
	glDrawPixels(buffer->w, buffer->h, GL_RGBA, GL_UNSIGNED_BYTE, buffer->pix);
}

int FLWindow::handle(int event) 
{

	switch (event)
	{
	case FL_KEYBOARD: {
		KeyboardHandle();
		return 0;
	}
	case FL_MOVE: {
		int u = Fl::event_x();
		int v = Fl::event_y();
		cout << "TRI: "<< buffer->GetId(u, v) << " fv: "<< scene->views[0]->GetPPC()->GetFocalLength() << " "<< u << ", "<< v << "         \r";
		return 0;
	}
	default:
		break;
	}
	return 0;
}

void FLWindow::KeyboardHandle() 
{
	int key = Fl::event_key();
	switch (key) {
	case FL_Up: {
		//scene->views[0]->GetPPC()->ZoomFocalLength(1.01f);
		scene->tmeshes[0].GetMaterial().kSpecular += 0.1f;
		scene->Render();
		Fl::check();
		break;
	}
	case FL_Down: {
		//scene->views[0]->GetPPC()->ZoomFocalLength(.99f);
		scene->tmeshes[0].GetMaterial().kSpecular += 0.1f;
		scene->Render();
		Fl::check();
		break;
	}
	case FL_Right: {
		scene->tmeshes[0].Rotate(Vec3d::ZEROS, Vec3d::YAXIS, 5);
		Fl::check();
		scene->Render();
		break;
	}
	case FL_Left: {
		scene->tmeshes[0].Rotate(Vec3d::ZEROS, Vec3d::YAXIS, -5);
		Fl::check();
		scene->Render();
		break;
	}
	}
}

