#ifndef WINDOW_H_
#define WINDOW_H_

#include <raylib.h>
#include <raymath.h>
#include <stddef.h>

typedef struct Window Window;

typedef size_t WindowHandle;

typedef void (*WindowUpdateCallback)(WindowHandle);
typedef void (*WindowDrawCallback)(WindowHandle);

WindowHandle create_window(const char *title, Vector2 pos, Vector2 size,
                           WindowUpdateCallback update_callback,
                           WindowDrawCallback draw_callback);
void free_window(WindowHandle window_handle);

void update_windows();
void draw_windows();

#endif // WINDOW_H_
