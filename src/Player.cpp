#include "Player.h"

#include <cmath>

#include "Game.h"
#include "World.h"
#define MOUSE_SPEED 0.1

Player::Player(){
    m_camera = { 0 };
    m_camera.position = (Vector3){ 20.0f, World::WORLD_CHUNK_HEIGHT*16 + 1, 10.0f }; // Camera position
    m_camera.target = (Vector3){ 0.0f, 0.0f, 0.0f };      // Camera looking at point
    m_camera.target =
    m_camera.up = (Vector3){ 0.0f, 1.0f, 0.0f };          // Camera up vector (rotation towards target)
    m_camera.fovy = 90;  // Camera field-of-view Y
    m_camera.projection = CAMERA_PERSPECTIVE;             // Camera projection type

    m_block_in_inventory = World::BLOCK_MATERIALS::STONE;
    m_reach = 5.0f;

    m_speed_adjust = 20;
    m_camera_speed = {0 , 0, 0};
}

void Player::update_position(){
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
    m_speed_adjust += 1 * GetMouseWheelMove();

    UpdateCameraPro(&m_camera, {m_camera_speed.x * GetFrameTime(),
        m_camera_speed.y * GetFrameTime(),
        m_camera_speed.z * GetFrameTime()},
        {float(mouseDelta.x*MOUSE_SPEED),
            float(mouseDelta.y*MOUSE_SPEED), 0}, 0);
    if (IsKeyPressed(KEY_ONE)) m_block_in_inventory = World::BLOCK_MATERIALS::DIRT;
    if (IsKeyPressed(KEY_TWO)) m_block_in_inventory = World::BLOCK_MATERIALS::GRASS_BLOCK;
    if (IsKeyPressed(KEY_THREE)) m_block_in_inventory = World::BLOCK_MATERIALS::STONE;
    if (IsKeyPressed(KEY_FOUR)) m_block_in_inventory = World::BLOCK_MATERIALS::OAK_PLANKS;
    if (IsKeyPressed(KEY_FIVE)) m_block_in_inventory = World::BLOCK_MATERIALS::OAK_LOG;

}

RayResult Player::handle_interaction(){
    Ray ray = GetScreenToWorldRay({
        static_cast<float>(GetScreenWidth()) / 2.0f,
        static_cast<float>(GetScreenHeight()) / 2.0f
    }, m_camera);
    RayResult ray_result = perform_ray_dda(ray, m_reach, Game::Get().m_world);
    if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT)){

        int block_x = static_cast<int>(floor(ray_result.hit_block.x));
        int block_y = static_cast<int>(floor(ray_result.hit_block.y));
        int block_z = static_cast<int>(floor(ray_result.hit_block.z));
        int in_chunk_x = block_x & 15;
        int in_chunk_y = block_y & 15;
        int in_chunk_z = block_z & 15;

        World::ChunkPos hit_chunk = World::get_chunk_position(ray_result.hit_block);

        Game::Get().m_world.set_block_material(block_x, block_y, block_z, World::BLOCK_MATERIALS::AIR );

        Game::Get().m_renderer.update_block_meshes(hit_chunk, in_chunk_x, in_chunk_y, in_chunk_z);

    }
    if (IsMouseButtonPressed(MOUSE_BUTTON_RIGHT)){
        int block_x = static_cast<int>(floor(ray_result.previous_block.x));
        int block_y = static_cast<int>(floor(ray_result.previous_block.y));
        int block_z = static_cast<int>(floor(ray_result.previous_block.z));
        int in_chunk_x = block_x & 15;
        int in_chunk_y = block_y & 15;
        int in_chunk_z = block_z & 15;
        World::ChunkPos hit_chunk = World::get_chunk_position(ray_result.previous_block);
        Game::Get().m_world.set_block_material(block_x, block_y, block_z, m_block_in_inventory );

        Game::Get().m_renderer.update_block_meshes(hit_chunk, in_chunk_x, in_chunk_y, in_chunk_z);



    }
    return ray_result;
}
