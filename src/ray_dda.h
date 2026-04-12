#pragma once
#include "raylib.h"
#include "World.h"
struct RayResult {
    bool did_hit = false;
    Vector3 previous_block;
    Vector3 hit_block;
};


RayResult perform_ray_dda(Ray ray, float max_distance, World& world);