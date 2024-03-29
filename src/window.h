#ifndef WINDOW_H_
#define WINDOW_H_

#include <raylib.h>
#include <raymath.h>
#include <stddef.h>

typedef struct Window Window;

typedef size_t WindowHandle;

typedef void (*WindowUpdateCallback)(Window *);
typedef void (*WindowDrawCallback)(Window *);

struct Window {
  WindowHandle id;
  const char *title;
  Vector2 pos;
  Vector2 size;
  WindowUpdateCallback update_callback;
  WindowDrawCallback draw_callback;
  RenderTexture render_texture;
};

WindowHandle create_window(const char *title, Vector2 pos, Vector2 size,
                           WindowUpdateCallback update_callback,
                           WindowDrawCallback draw_callback);
void free_window(WindowHandle window_handle);

void update_windows();
void draw_windows();

#endif // WINDOW_H_
