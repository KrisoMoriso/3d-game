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
    m_player.update_physics();
    m_player.update_position();
    RayResult ray = m_player.handle_interaction();
    BeginDrawing();
    ClearBackground(SKYBLUE);
    BeginMode3D(m_player.m_camera);
        m_renderer.render_chunks(m_player.m_camera.position);
        if (ray.did_hit) DrawCubeWiresV({ray.hit_block.x + 0.5f,ray.hit_block.y + 0.5f,ray.hit_block.z + 0.5f}, {1.0f, 1.0f, 1.0f}, WHITE);
    EndMode3D();
    DrawText(TextFormat("%f %f %f", m_player.m_camera.position.x, m_player.m_camera.position.y, m_player.m_camera.position.z), 50, 50, 25, BLACK);
    DrawText(World::BLOCK_MATERIALS::match_material(m_player.m_block_in_inventory).c_str(), 100, 200, 25, BLACK);
    DrawFPS(50, 100);
    DrawTexture(ResourceManager::Get().CROSSHAIR_TEXTURE, GetScreenWidth() / 2 - 3, GetScreenHeight() / 2 - 3, WHITE);
    EndDrawing();


}
