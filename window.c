#include "window.h"
#include "nob.h"

static void draw_text_centered(const char *text, int font_size, Vector2 pos,
                               Color color) {
  const int width = MeasureText(text, font_size), height = font_size;
  DrawText(text, (int)(pos.x - width / 2.0f), (int)(pos.y - height / 2.0f),
           font_size, color);
}

struct Window {
  WindowHandle id;
  const char *title;
  Vector2 pos;
  Vector2 size;
  WindowUpdateCallback update_callback;
  WindowDrawCallback draw_callback;
  RenderTexture render_texture;
};

static struct {
  Window *items;
  size_t count;
  size_t capacity;
} windows = {0};

WindowHandle create_window(const char *title, Vector2 pos, Vector2 size,
                           WindowUpdateCallback update_callback,
                           WindowDrawCallback draw_callback) {
  Window w = {
      .id = windows.count,
      .title = title,
      .pos = pos,
      .size = size,
      .update_callback = update_callback,
      .draw_callback = draw_callback,
      .render_texture = LoadRenderTexture((int)w.size.x, (int)w.size.y),
  };
  nob_da_append(&windows, w);
  return w.id;
}

static size_t find_window_from_handle(WindowHandle window_handle) {
  for (size_t i = 0; i < windows.count; ++i)
    if (windows.items[i].id == window_handle)
      return i;
  return windows.count + 1;
}

void free_window(WindowHandle window_handle) {
  size_t index = find_window_from_handle(window_handle);
  if (index > windows.count)
    return;
  UnloadRenderTexture(windows.items[index].render_texture);
  for (size_t i = index; i < windows.count - 1; ++i)
    windows.items[i] = windows.items[i + 1];
  windows.count -= 1;
}

static bool is_dragging = false;
static Vector2 drag_offset = {0};

void update_windows() {
  const Vector2 mouse_position = GetMousePosition();
  size_t clicked = windows.count;
  for (size_t i = 0; i < windows.count; ++i) {
    Window *w = windows.items + i;

    if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON) &&
        CheckCollisionPointRec(mouse_position, (Rectangle){.x = w->pos.x,
                                                           .y = w->pos.y,
                                                           .width = w->size.x,
                                                           .height = 20.0f})) {
      clicked = i;
    } else if (is_dragging) {
      if (IsMouseButtonDown(MOUSE_LEFT_BUTTON))
        windows.items[windows.count - 1].pos =
            Vector2Clamp(Vector2Subtract(mouse_position, drag_offset),
                         (Vector2){.x = 0.0f, .y = 0.0f},
                         (Vector2){.x = 800.0f - w->size.x,
                                   .y = 600.0f - w->size.y - 20.0f});
      else {
        is_dragging = false;
        drag_offset = (Vector2){0};
      }
    }

    if (w->update_callback)
      w->update_callback(w->id);
  }
  if (clicked < windows.count) {
    Window temp = windows.items[clicked];
    for (size_t j = clicked; j < windows.count - 1; j++)
      windows.items[j] = windows.items[j + 1];
    windows.items[windows.count - 1] = temp;
    is_dragging = true;
    drag_offset = Vector2Subtract(mouse_position, temp.pos);
  }
}

void draw_windows() {
  for (size_t i = 0; i < windows.count; ++i) {
    Window *w = windows.items + i;
    DrawRectangleV(w->pos, (Vector2){.x = w->size.x, .y = 20}, LIGHTGRAY);
    draw_text_centered(
        w->title, 10,
        (Vector2){.x = w->pos.x + w->size.x / 2.0f, .y = w->pos.y + 10.0f},
        BLACK);
    BeginTextureMode(w->render_texture);
    if (w->draw_callback)
      w->draw_callback(w->id);
    EndTextureMode();
    DrawTextureRec(
        w->render_texture.texture,
        (Rectangle){
            .x = 0, .y = w->size.y, .width = w->size.x, .height = -w->size.y},
        (Vector2){.x = w->pos.x, .y = w->pos.y + 20.0f}, WHITE);
  }
}
