#include "window.h"
#include <math.h>
#include <raylib.h>
#include <raymath.h>

#define BALL_RADIUS 15.0
#define GRAVITY 1500.0

static Vector2 ball_position = {20.0f, 20.0f};
static Vector2 ball_velocity = {200, 200};

void ball_window_update(Window *w) {
  const float dt = GetFrameTime();
  ball_velocity.y += GRAVITY * dt;
  float x = ball_position.x + ball_velocity.x * dt;
  if (x - BALL_RADIUS < 0.0 || x + BALL_RADIUS >= w->size.x) {
    ball_velocity.x *= -1.0f;
  } else {
    ball_position.x = x;
  }
  float y = ball_position.y + ball_velocity.y * dt;
  if (y - BALL_RADIUS < 0.0 || y + BALL_RADIUS >= w->size.y) {
    ball_velocity.y *= -1.0f;
  } else {
    ball_position.y = y;
  }
}

void ball_window_draw(Window *w) {
  (void)w;
  ClearBackground((Color){20, 20, 20, 255});
  DrawCircleV(ball_position, BALL_RADIUS, RED);
}

static Vector2 pos = {0};

void fallback_window_draw(Window *w) {
  ClearBackground(BLACK);
  switch (w->id) {
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
      create_window("Tsoding Ball Example", (Vector2){.x = 65.0f, .y = 40.0f},
                    (Vector2){.x = 300.0f, .y = 225.0f}, ball_window_update,
                    ball_window_draw);
  WindowHandle win2 = create_window(
      "Placeholder Blue Square Example", (Vector2){.x = 480.0f, .y = 80.0f},
      (Vector2){.x = 200.0f, .y = 200.0f}, NULL, fallback_window_draw);
  WindowHandle win3 = create_window(
      "Placeholder Yellow Square Example", (Vector2){.x = 120.0f, .y = 300.0f},
      (Vector2){.x = 200.0f, .y = 200.0f}, NULL, fallback_window_draw);
  WindowHandle win4 = create_window(
      "Placeholder Green Circle Example", (Vector2){.x = 480.0f, .y = 300.0f},
      (Vector2){.x = 200.0f, .y = 200.0f}, NULL, fallback_window_draw);

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

  free_window(win4);
  free_window(win3);
  free_window(win2);
  free_window(win1);

  CloseWindow();
  return 0;
}
