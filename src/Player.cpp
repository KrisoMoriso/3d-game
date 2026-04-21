#include "Player.h"

#include <cmath>
#include "raymath.h"
#include "Game.h"
#include "World.h"
#define MOUSE_SPEED 0.1

Player::Player(){
    m_camera = { 0 };
    m_camera.position = (Vector3){ -150.0f, World::WORLD_CHUNK_HEIGHT*16 - 10, -193.5f }; // Camera position
    m_position = m_camera.position;
    m_camera.target = (Vector3){ 0.0f, 0.0f, 0.0f };      // Camera looking at point
    m_camera.target =
    m_camera.up = (Vector3){ 0.0f, 1.0f, 0.0f };          // Camera up vector (rotation towards target)
    m_camera.fovy = 90;  // Camera field-of-view Y
    m_camera.projection = CAMERA_PERSPECTIVE;             // Camera projection type

    m_block_in_inventory = World::BLOCK_MATERIALS::STONE;
    m_reach = 5.0f;

    m_speed_adjust = 20;
    m_velocity = {0 , 0, 0};
}

void Player::update_position(){
    Vector2 mouseDelta = GetMouseDelta();
    SetMousePosition(250, 250);

    m_speed_adjust += 1 * GetMouseWheelMove();

    UpdateCameraPro(&m_camera, {0,0, 0},
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
    if (!ray_result.did_hit) return ray_result;
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

bool Player::check_collision_AABB(){
    World& world = Game::Get().m_world;

    float min_x = m_position.x - (m_width / 2.0f);
    float max_x = m_position.x + (m_width / 2.0f);

    float min_y = m_position.y;
    float max_y = m_position.y + m_height;

    float min_z = m_position.z - (m_width / 2.0f);
    float max_z = m_position.z + (m_width / 2.0f);


    int block_min_x = get_voxel_coordinate(min_x);
    int block_max_x = get_voxel_coordinate(max_x);

    int block_min_y = get_voxel_coordinate(min_y);
    int block_max_y = get_voxel_coordinate(max_y);

    int block_min_z = get_voxel_coordinate(min_z);
    int block_max_z = get_voxel_coordinate(max_z);

    for (int x = block_min_x; x <= block_max_x; ++x) {
        for (int y = block_min_y; y <= block_max_y; ++y) {
            for (int z = block_min_z; z <= block_max_z; ++z) {
                if (world.get_block_material(x, y, z) != World::BLOCK_MATERIALS::AIR) {
                    return true;
                }
            }
        }
    }
    return false;
}
void Player::update_physics(bool enable_player_collision) {
    World& world = Game::Get().m_world;
    float dt = GetFrameTime();


    Vector3 forward = Vector3Subtract(m_camera.target, m_camera.position);
    forward.y = 0.0f;
    forward = Vector3Normalize(forward);


    Vector3 right = { -forward.z, 0.0f, forward.x };


    Vector3 movement = { 0 };

    if (IsKeyDown(KEY_W)) movement = Vector3Add(movement, Vector3Scale(forward, m_speed_adjust * dt));
    if (IsKeyDown(KEY_S)) movement = Vector3Subtract(movement, Vector3Scale(forward, m_speed_adjust * dt));
    if (IsKeyDown(KEY_D)) movement = Vector3Add(movement, Vector3Scale(right, m_speed_adjust * dt));
    if (IsKeyDown(KEY_A)) movement = Vector3Subtract(movement, Vector3Scale(right, m_speed_adjust * dt));

    if (IsKeyDown(KEY_SPACE)) movement.y += m_speed_adjust * dt;
    if (IsKeyDown(KEY_LEFT_SHIFT)) movement.y -= m_speed_adjust * dt;


    m_position.x += movement.x;

    //X
    if (check_collision_AABB() and enable_player_collision){
        if (movement.x > 0){
            //X+
            m_position.x = get_voxel_coordinate(m_position.x + m_width / 2.0f) - (m_width / 2.0f) - 0.001f;
        }
        else if (movement.x < 0){
            //X-
            m_position.x = get_voxel_coordinate(m_position.x - m_width / 2.0f) + 1.0f + (m_width / 2.0f) + 0.001f;
        }
        m_velocity.x = 0;
    }

    //Y
    m_position.y += movement.y;
    if (check_collision_AABB() and enable_player_collision){
        if (movement.y > 0){
            // Y+
            m_position.y = get_voxel_coordinate(m_position.y + m_height) - m_height - 0.001f;
        }
        else if (movement.y < 0){
            //Y-
            m_position.y = get_voxel_coordinate(m_position.y) + 1.0f;
        }
        m_velocity.y = 0;
    }

    // Z
    m_position.z += movement.z;
    if (check_collision_AABB() and enable_player_collision){
        if (movement.z > 0){
            // Z+
            m_position.z = get_voxel_coordinate(m_position.z + m_width / 2.0f) - (m_width / 2.0f) - 0.001f;
        }
        else if (movement.z < 0){
            // Z-
            m_position.z = get_voxel_coordinate(m_position.z - m_width / 2.0f) + 1.0f + (m_width / 2.0f) + 0.001f;
        }
        m_velocity.z = 0;
    }


    Vector3 view_dir = Vector3Subtract(m_camera.target, m_camera.position);

    m_camera.position = { m_position.x, m_position.y + 1.6f, m_position.z };

    m_camera.target = Vector3Add(m_camera.position, view_dir);
}