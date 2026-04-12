#include "Game.h"

#include <memory>

#include "Block.h"
#include "raylib.h"
#include "Renderer.h"
#include "ResourceManager.h"
#include "rlgl.h"

void Game::init() {
    InitWindow(1700, 900, "3d_game");
    // ToggleBorderlessWindowed();
    SetTargetFPS(165);
    HideCursor();
    ResourceManager::Get().init();
    m_renderer.update_mesh(m_player.m_camera.position);

}

void Game::main_loop() {
    m_renderer.update_mesh(m_player.m_camera.position);
    m_world.generate_world(m_player.m_camera.position);
    BeginDrawing();
    ClearBackground(SKYBLUE);
    BeginMode3D(m_player.m_camera);
        m_renderer.render_chunks(m_player.m_camera.position);
    EndMode3D();
    DrawText(TextFormat("%f %f %f", m_player.m_camera.position.x, m_player.m_camera.position.y, m_player.m_camera.position.z), 50, 50, 25, BLACK);
    DrawFPS(50, 100);
    EndDrawing();

    m_player.update_position();
    m_player.handle_interaction();

}
