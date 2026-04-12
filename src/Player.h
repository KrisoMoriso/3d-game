#pragma once
#include "raylib.h"
#include "ray_dda.h"


class Player {
public:
    Camera m_camera;
    unsigned short m_block_in_inventory;
    float m_reach;
    float m_speed_adjust;
    Vector3 m_camera_speed;
    Player();
    void update_position();
    RayResult handle_interaction();
};
