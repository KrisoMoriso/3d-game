#pragma once
#include "raylib.h"
#include "ray_dda.h"


class Player {
public:
    Vector3 m_position;
    float m_height = 1.8f;
    float m_width = 0.6f;
    Camera m_camera;
    unsigned short m_block_in_inventory;
    float m_reach;
    float m_speed_adjust;
    Vector3 m_velocity;
    Player();
    void update_position();
    RayResult handle_interaction();
    static int get_voxel_coordinate(float val){
        return static_cast<int>(std::floor(val));
    }
    bool check_collision_AABB();
    void update_physics(bool enable_player_collision);
};
