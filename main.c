#include <math.h>
#include <raylib.h>
#include <raymath.h>
#include <stddef.h>

void draw_text_centered(const char *text, int font_size, Vector2 pos,
                        Color color) {
  const int width = MeasureText(text, font_size), height = font_size;
  DrawText(text, (int)(pos.x - width / 2.0f), (int)(pos.y - height / 2.0f),
           font_size, color);
}

typedef struct Window Window;

typedef void (*WindowUpdateCallback)(Window *);
typedef void (*WindowDrawCallback)(Window *);

struct Window {
  Vector2 pos;
  Vector2 size;
  const char *title;
  WindowUpdateCallback update_callback;
  WindowDrawCallback draw_callback;
  RenderTexture render_texture;
  size_t id;
};

static bool is_dragging = false;
static Vector2 drag_offset = {0};

void update_windows(Window *windows, size_t window_count) {
  const Vector2 mouse_position = GetMousePosition();
  size_t clicked = window_count;
  for (size_t i = 0; i < window_count; ++i) {
    Window *w = windows + i;

    if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON) &&
        CheckCollisionPointRec(mouse_position, (Rectangle){.x = w->pos.x,
                                                           .y = w->pos.y,
                                                           .width = w->size.x,
                                                           .height = 20.0f})) {
      clicked = i;
    } else if (is_dragging) {
      if (IsMouseButtonDown(MOUSE_LEFT_BUTTON))
        windows[window_count - 1].pos =
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
      w->update_callback(w);
  }
  if (clicked < window_count) {
    Window temp = windows[clicked];
    for (size_t j = clicked; j < window_count - 1; j++)
      windows[j] = windows[j + 1];
    windows[window_count - 1] = temp;
    is_dragging = true;
    drag_offset = Vector2Subtract(mouse_position, temp.pos);
  }
}

void draw_windows(Window *windows, size_t window_count) {
  for (size_t i = 0; i < window_count; ++i) {
    Window *w = windows + i;
    DrawRectangleV(w->pos, (Vector2){.x = w->size.x, .y = 20}, LIGHTGRAY);
    draw_text_centered(
        w->title, 10,
        (Vector2){.x = w->pos.x + w->size.x / 2.0f, .y = w->pos.y + 10.0f},
        BLACK);
    BeginTextureMode(w->render_texture);
    if (w->draw_callback)
      w->draw_callback(w);
    EndTextureMode();
    DrawTextureRec(
        w->render_texture.texture,
        (Rectangle){
            .x = 0, .y = w->size.y, .width = w->size.x, .height = -w->size.y},
        (Vector2){.x = w->pos.x, .y = w->pos.y + 20.0f}, WHITE);
  }
}

static Vector2 pos = {0};

void my_window_draw(Window *w);

static Window windows[] = {
    {
        .pos = {.x = 120.0f, .y = 80.0f},
        .size = {.x = 200.0f, .y = 200.0f},
        .title = "Red Circle Window",
        .update_callback = NULL,
        .draw_callback = my_window_draw,
        .render_texture = {0},
        .id = 0,
    },
    {
        .pos = {.x = 480.0f, .y = 80.0f},
        .size = {.x = 200.0f, .y = 200.0f},
        .title = "Blue Square Window",
        .update_callback = NULL,
        .draw_callback = my_window_draw,
        .render_texture = {0},
        .id = 1,
    },
    {
        .pos = {.x = 120.0f, .y = 300.0f},
        .size = {.x = 200.0f, .y = 200.0f},
        .title = "Yellow Square Window",
        .update_callback = NULL,
        .draw_callback = my_window_draw,
        .render_texture = {0},
        .id = 2,
    },
    {
        .pos = {.x = 480.0f, .y = 300.0f},
        .size = {.x = 200.0f, .y = 200.0f},
        .title = "Green Circle Window",
        .update_callback = NULL,
        .draw_callback = my_window_draw,
        .render_texture = {0},
        .id = 3,
    },
};

static const size_t window_count = sizeof(windows) / sizeof(Window);

void my_window_draw(Window *w) {
  ClearBackground(BLACK);
  switch (w->id) {
    case 0:
      DrawCircleV(pos, 20.0f, RED);
      break;
    case 1:
      DrawRectangleV(Vector2Subtract(pos, (Vector2){.x = 20.0f, .y = 20.0f}), (Vector2){.x = 40.0f, .y = 40.0f}, BLUE);
      break;
    case 2:
      DrawRectangleV(Vector2Subtract(pos, (Vector2){.x = 20.0f, .y = 20.0f}), (Vector2){.x = 40.0f, .y = 40.0f}, YELLOW);
      break;
    case 3:
      DrawCircleV(pos, 20.0f, GREEN);
      break;
    default: break;
  }
}

void init_windows() {
  for (size_t i = 0; i < window_count; ++i) {
    Window *w = windows + i;
    w->render_texture = LoadRenderTexture((int)w->size.x, (int)w->size.y);
  }
}

void deinit_windows() {
  for (size_t i = 0; i < window_count; ++i) {
    Window *w = windows + i;
    UnloadRenderTexture(w->render_texture);
  }
}

int main(void) {
  InitWindow(800, 600, "Fake-OS");
  SetTargetFPS(60);

  init_windows();

  while (!WindowShouldClose()) {
    update_windows(windows, window_count);
    const float frame_time = GetFrameTime();
    const float time = GetTime();
    pos.x += 60.0f * frame_time;
    if (pos.x > 260.0f)
      pos.x = -60.0f;
    pos.y = 100.0f + sinf(time * 2.0f) * 30.0f;
    BeginDrawing();
    ClearBackground(DARKGRAY);
    draw_windows(windows, window_count);
    EndDrawing();
  }

  deinit_windows();

  CloseWindow();
  return 0;
}
