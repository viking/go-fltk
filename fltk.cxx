#include <fltk/Window.h>
#include <fltk/Widget.h>
#include <fltk/Input.h>
#include <fltk/TextEditor.h>
#include <fltk/run.h>
#include <fltk/events.h>
#include <fltk/Threads.h>
#include "fltk.h"
#include <stdio.h>

#define MAX_EVENT_TYPES 25

Widget *go_fltk_callback_widget = 0;
Widget *go_fltk_event_widget;
int go_fltk_event;
int go_fltk_event_state;
int go_fltk_event_button;
int go_fltk_event_clicks;
int go_fltk_event_dx;
int go_fltk_event_dy;
int go_fltk_event_key;
int go_fltk_event_x;
int go_fltk_event_y;
int go_fltk_event_x_root;
int go_fltk_event_y_root;
int go_fltk_event_stolen;

void set_event_fields() {
  go_fltk_event_widget = fltk::belowmouse();
  go_fltk_event = fltk::event();
  go_fltk_event_state = fltk::event_state();
  go_fltk_event_button = fltk::event_button();
  go_fltk_event_clicks = fltk::event_clicks();
  go_fltk_event_dx = fltk::event_dx();
  go_fltk_event_dy = fltk::event_dy();
  go_fltk_event_key = fltk::event_key();
  go_fltk_event_x = fltk::event_x();
  go_fltk_event_y = fltk::event_y();
  go_fltk_event_x_root = fltk::event_x_root();
  go_fltk_event_y_root = fltk::event_y_root();
}

static void respond();

static void lock() {
  printf("LOCK\n");
  fltk::lock();
}

static void unlock() {
  printf("UNLOCK\n");
  fltk::unlock();
}

class EventStealer {
public:
  int stealEvents;
  virtual int continue_event() = 0;
  int handle(int event) {
    bool stolen = (stealEvents & (1 << event)) != 0;

    if (stolen) {
      go_fltk_event_stolen = 1;
      go_fltk_event_widget = dynamic_cast<Widget *>(this);
      respond();
    }
    int i = continue_event();
    if (!stolen) respond();
    return i;
  }
};

class GWidget : public EventStealer, public fltk::Widget {
public:
  GWidget(int x, int y, int w, int h, const char *label) : fltk::Widget(x, y, w, h, label) {}
  int handle(int event) {return EventStealer::handle(event);}
  int continue_event() {return fltk::Widget::handle(fltk::event());}
};

class GInput : public EventStealer, public fltk::Input {
public:
  GInput(int x, int y, int w, int h, const char *label) : fltk::Input(x, y, w, h, label) {}
  int handle(int event) {return EventStealer::handle(event);}
  int continue_event() {return fltk::Input::handle(fltk::event());}
};

class GTextEditor : public EventStealer, public fltk::TextEditor {
public:
  GTextEditor(int x, int y, int w, int h, const char *label) : fltk::TextEditor(x, y, w, h, label) {}
  int handle(int event) {return EventStealer::handle(event);}
  int continue_event() {return fltk::TextEditor::handle(fltk::event());}
};

static void notify_callback(Widget *w, void *data) {
  go_fltk_callback_widget = w;
}

#define LOCK(code) {fltk::lock(); printf("LOCKED\n"); code; fltk::awake(); fltk::unlock(); printf("UNLOCKED\n");}

Box *go_fltk_get_UP_BOX() LOCK({return fltk::UP_BOX;})
Font *go_fltk_get_HELVETICA_BOLD_ITALIC() LOCK({return  fltk::HELVETICA_BOLD_ITALIC;})
LabelType *go_fltk_get_SHADOW_LABEL() LOCK({return fltk::SHADOW_LABEL;})
void go_fltk_run() LOCK({fltk::run();})
Window *go_fltk_new_Window(int w, int h) LOCK({return new Window(w, h);})
Widget *go_fltk_new_Widget(int x, int y, int w, int h, const char *text) LOCK({return new GWidget(x, y, w, h, text);})
Input *go_fltk_new_Input(int x, int y, int w, int h, const char *text) LOCK({return new GInput(x, y, w, h, text);})
TextEditor *go_fltk_new_TextEditor(int x, int y, int w, int h, const char *text) LOCK({return new GTextEditor(x, y, w, h, text);})
void go_fltk_Widget_steal_events(Widget *w, int events) LOCK({dynamic_cast<EventStealer*>(w)->stealEvents = events;})
void go_fltk_Widget_continue_event(Widget *w) LOCK({dynamic_cast<EventStealer*>(w)->continue_event();})
void go_fltk_Group_begin(Group *g) LOCK({g->begin();})
void go_fltk_Group_end(Group *g) LOCK({g->end();})
void go_fltk_Group_resizable(Group *g, Widget *w) LOCK({g->resizable(w);})
void go_fltk_Window_show(Window *w, int argc, void *argv) LOCK({w->show(argc, (char **)argv);})
void go_fltk_Widget_box(Widget *w, Box *box) LOCK({w->box(box);})
void go_fltk_Widget_callback(Widget *w) LOCK({w->callback(notify_callback);})
void go_fltk_Widget_labelfont(Widget *w, Font *font) LOCK({w->labelfont(font);})
void go_fltk_Widget_labelsize(Widget *w, int size) LOCK({w->labelsize(size);})
void go_fltk_Widget_labeltype(Widget *w, LabelType *type) LOCK({w->labeltype(type);})
int go_fltk_Widget_x(Widget *w) LOCK({return w->x();})
int go_fltk_Widget_y(Widget *w) LOCK({return w->y();})
int go_fltk_Widget_w(Widget *w) LOCK({return w->w();})
int go_fltk_Widget_h(Widget *w) LOCK({return w->h();})
//*
int go_fltk_wait_forever() {
  printf("TICK\n");
  go_fltk_callback_widget = 0;
  go_fltk_event_stolen = 0;
  int i = fltk::wait();

  set_event_fields();
  return i;
}
int go_fltk_wait(double time) {
  printf("TICK\n");
  go_fltk_callback_widget = 0;
  go_fltk_event_stolen = 0;
  int i = fltk::wait(time);

  set_event_fields();
  return i;
}
//*/

////////////////
/// CHANNELS ///
////////////////

static fltk::SignalMutex eventMutex, continueMutex;
static bool running = false;

static void respond() {
  if (running) {
    printf("RESPOND -- WAITING\n");
    unlock();
    eventMutex.signal();
    continueMutex.wait();
    lock();
    fltk::awake();
  } else {
    printf("RESPOND -- NOT WAITING\n");
  }
}

static void *startFltk(void *data) {
  lock();
  fltk::awake();
  running = true;
  fltk::run();
}

void go_fltk_get_event() {eventMutex.wait();}
void go_fltk_continue_event() {continueMutex.signal();}
void go_fltk_init() {fltk::Thread t1; fltk::create_thread(t1, startFltk, 0);}
