#include "ray_dda.h"

#include <cmath>
#include <limits>
RayResult perform_ray_dda(Ray ray, float max_distance, World& world){
    RayResult ray_result = {false, {0,0,0}, {0,0,0}};
    int voxel_x = static_cast<int>(floor(ray.position.x));
    int voxel_y = static_cast<int>(floor(ray.position.y));
    int voxel_z = static_cast<int>(floor(ray.position.z));

    int step_x = ray.direction.x > 0 ? 1 : (ray.direction.x < 0 ? -1 : 0);
    int step_y = ray.direction.y > 0 ? 1 : (ray.direction.y < 0 ? -1 : 0);
    int step_z = ray.direction.z > 0 ? 1 : (ray.direction.z < 0 ? -1 : 0);

    float t_delta_x = step_x != 0 ? std::abs(1.0f / ray.direction.x) : std::numeric_limits<float>::max();
    float t_delta_y = step_y != 0 ? std::abs(1.0f / ray.direction.y) : std::numeric_limits<float>::max();
    float t_delta_z = step_z != 0 ? std::abs(1.0f / ray.direction.z) : std::numeric_limits<float>::max();

    float t_max_x = step_x > 0 ? (1.0f + voxel_x - ray.position.x) * t_delta_x
                                : (ray.position.x - voxel_x) * t_delta_x;
    float t_max_y = step_y > 0 ? (1.0f + voxel_y - ray.position.y) * t_delta_y
                                : (ray.position.y - voxel_y) * t_delta_y;
    float t_max_z = step_z > 0 ? (1.0f + voxel_z - ray.position.z) * t_delta_z
                                : (ray.position.z - voxel_z) * t_delta_z;

    int previous_block_x = voxel_x;
    int previous_block_y = voxel_y;
    int previous_block_z = voxel_z;

    float distance_travelled = 0.0f;

    while (distance_travelled < max_distance){
        auto current_block_material = world.get_block_material(voxel_x, voxel_y, voxel_z);
        if (current_block_material != World::BLOCK_MATERIALS::AIR){
            ray_result.did_hit = true;
            ray_result.hit_block = {float(voxel_x), float(voxel_y), float(voxel_z)};
            ray_result.previous_block = {float(previous_block_x), float(previous_block_y), float(previous_block_z)};
            return ray_result;
        }

        previous_block_x = voxel_x;
        previous_block_y = voxel_y;
        previous_block_z = voxel_z;

        if (t_max_x < t_max_y){
            if (t_max_x < t_max_z){
                voxel_x += step_x;
                distance_travelled = t_max_x;
                t_max_x += t_delta_x;
            } else {
                voxel_z += step_z;
                distance_travelled = t_max_z;
                t_max_z += t_delta_z;
            }
        } else{
            if (t_max_y < t_max_z){
                voxel_y += step_y;
                distance_travelled = t_max_y;
                t_max_y += t_delta_y;
            } else{
                voxel_z += step_z;
                distance_travelled = t_max_z;
                t_max_z += t_delta_z;
            }
        }
    }
    return ray_result;

}
