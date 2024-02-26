#include "window.h"
#include <math.h>
#include <raylib.h>
#include <raymath.h>

static Vector2 pos = {0};

void my_window_draw(WindowHandle window_handle) {
  ClearBackground(BLACK);
  switch (window_handle) {
  case 0:
    DrawCircleV(pos, 20.0f, RED);
    break;
  case 1:
    DrawRectangleV(Vector2Subtract(pos, (Vector2){.x = 20.0f, .y = 20.0f}),
                   (Vector2){.x = 40.0f, .y = 40.0f}, BLUE);
    break;
  case 2:
    DrawRectangleV(Vector2Subtract(pos, (Vector2){.x = 20.0f, .y = 20.0f}),
                   (Vector2){.x = 40.0f, .y = 40.0f}, YELLOW);
    break;
  case 3:
    DrawCircleV(pos, 20.0f, GREEN);
    break;
  default:
    break;
  }
}

int main(void) {
  InitWindow(800, 600, "Fake-OS");
  SetTargetFPS(60);

  WindowHandle win1 =
      create_window("Red Circle Window", (Vector2){.x = 120.0f, .y = 80.0f},
                    (Vector2){.x = 200.0f, .y = 200.0f}, NULL, my_window_draw);
  WindowHandle win2 =
      create_window("Blue Square Window", (Vector2){.x = 480.0f, .y = 80.0f},
                    (Vector2){.x = 200.0f, .y = 200.0f}, NULL, my_window_draw);
  WindowHandle win3 =
      create_window("Yellow Square Window", (Vector2){.x = 120.0f, .y = 300.0f},
                    (Vector2){.x = 200.0f, .y = 200.0f}, NULL, my_window_draw);
  WindowHandle win4 =
      create_window("Green Circle Window", (Vector2){.x = 480.0f, .y = 300.0f},
                    (Vector2){.x = 200.0f, .y = 200.0f}, NULL, my_window_draw);

  while (!WindowShouldClose()) {
    update_windows();
    const float frame_time = GetFrameTime();
    const float time = GetTime();
    pos.x += 60.0f * frame_time;
    if (pos.x > 260.0f)
      pos.x = -60.0f;
    pos.y = 100.0f + sinf(time * 2.0f) * 30.0f;
    BeginDrawing();
    ClearBackground(DARKGRAY);
    draw_windows();
    EndDrawing();
  }

  free_window(win1);
  free_window(win2);
  free_window(win3);
  free_window(win4);

  CloseWindow();
  return 0;
}
