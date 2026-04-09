#include "Game.h"

#include <memory>

#include "Block.h"
#include "raylib.h"
#include "Renderer.h"
#include "ResourceManager.h"
#include "rlgl.h"
#define AIR 0
#define DIRT 1
#define MOUSE_SPEED 0.1

void Game::init() {
    InitWindow(1700, 900, "3d_game");
    // ToggleBorderlessWindowed();
    SetTargetFPS(165);
    m_camera = { 0 };
    m_camera.position = (Vector3){ 20.0f, 105.0f, 10.0f }; // Camera position
    m_camera.target = (Vector3){ 0.0f, 0.0f, 0.0f };      // Camera looking at point
    m_camera.target =
    m_camera.up = (Vector3){ 0.0f, 1.0f, 0.0f };          // Camera up vector (rotation towards target)
    m_camera.fovy = 90;  // Camera field-of-view Y
    m_camera.projection = CAMERA_PERSPECTIVE;             // Camera projection type
    m_speed_adjust = 0.1;
    m_camera_speed = {0 , 0, 0};
    HideCursor();
    ResourceManager::Get().init();
    m_renderer.update_mesh();

}

void Game::main_loop() {
    BeginDrawing();
    ClearBackground(SKYBLUE);
    BeginMode3D(m_camera);
        DrawGrid(32, 1);
        m_renderer.render_chunks();


    EndMode3D();
    DrawText(TextFormat("%f %f %f", m_camera.position.x, m_camera.position.y, m_camera.position.z), 50, 50, 25, BLACK);
    DrawFPS(50, 100);
    EndDrawing();
    Vector2 mouseDelta = GetMouseDelta();
    SetMousePosition(250, 250);
    if (IsKeyDown(KEY_W)) {m_camera_speed.x = m_speed_adjust;}
    if (IsKeyDown(KEY_S)) {m_camera_speed.x = -m_speed_adjust;}
    if (!IsKeyDown(KEY_W) and !IsKeyDown(KEY_S)) {m_camera_speed.x = 0;}

    if (IsKeyDown(KEY_A)) {m_camera_speed.y = -m_speed_adjust;}
    if (IsKeyDown(KEY_D)) {m_camera_speed.y = m_speed_adjust;}
    if (!IsKeyDown(KEY_A) and !IsKeyDown(KEY_D)) {m_camera_speed.y = 0;}

    if (IsKeyDown(KEY_LEFT_SHIFT)) {m_camera_speed.z = -m_speed_adjust;}
    if (IsKeyDown(KEY_SPACE)) {m_camera_speed.z = m_speed_adjust;}
    if (!IsKeyDown(KEY_LEFT_SHIFT) and !IsKeyDown(KEY_SPACE)) {m_camera_speed.z = 0;}

    m_speed_adjust += 0.01 * GetMouseWheelMove();

    UpdateCameraPro(&m_camera, {m_camera_speed.x, m_camera_speed.y, m_camera_speed.z}, {float(mouseDelta.x*MOUSE_SPEED), float(mouseDelta.y*MOUSE_SPEED), 0}, 0);
}
