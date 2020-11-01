// generated by Fast Light User Interface Designer (fluid) version 1.0304

#include "gui.h"

void GUI::cb_Run_i(Fl_Button*, void*) {
  RunInvis_cb();
}
void GUI::cb_Run(Fl_Button* o, void* v) {
  ((GUI*)(o->parent()->user_data()))->cb_Run_i(o,v);
}

void GUI::cb_Run1_i(Fl_Button*, void*) {
  RunShadows_cb();
}
void GUI::cb_Run1(Fl_Button* o, void* v) {
  ((GUI*)(o->parent()->user_data()))->cb_Run1_i(o,v);
}

void GUI::cb_Run2_i(Fl_Button*, void*) {
  RunProjector_cb();
}
void GUI::cb_Run2(Fl_Button* o, void* v) {
  ((GUI*)(o->parent()->user_data()))->cb_Run2_i(o,v);
}

void GUI::cb_Run3_i(Fl_Button*, void*) {
  RunReflections_cb();
}
void GUI::cb_Run3(Fl_Button* o, void* v) {
  ((GUI*)(o->parent()->user_data()))->cb_Run3_i(o,v);
}
#include "scene.h"

GUI::GUI() {
  { uiw = new Fl_Double_Window(415, 69, "GUI");
    uiw->user_data((void*)(this));
    { Fl_Button* o = new Fl_Button(315, 15, 95, 40, "Run Invisibility");
      o->selection_color(FL_DARK_RED);
      o->callback((Fl_Callback*)cb_Run);
    } // Fl_Button* o
    { Fl_Button* o = new Fl_Button(115, 15, 95, 40, "Run Shadows");
      o->callback((Fl_Callback*)cb_Run1);
    } // Fl_Button* o
    { Fl_Button* o = new Fl_Button(215, 15, 95, 40, "Run Projector");
      o->callback((Fl_Callback*)cb_Run2);
    } // Fl_Button* o
    { Fl_Button* o = new Fl_Button(5, 15, 105, 40, "Run Reflections");
      o->callback((Fl_Callback*)cb_Run3);
    } // Fl_Button* o
    uiw->end();
  } // Fl_Double_Window* uiw
}

int main(int argc, char **argv) {
  scene = new Scene;
  return Fl::run();
}

void GUI::show() {
  uiw->show();
}

void GUI::RunInvis_cb() {
  scene->RunInvisibility();
}

void GUI::RunShadows_cb() {
  scene->RunShadows();
}

void GUI::RunProjector_cb() {
  scene->RunProjector();
}

void GUI::RunReflections_cb() {
  scene->RunReflections();
}
