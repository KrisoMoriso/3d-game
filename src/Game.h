#pragma once
#include <memory>

#include "Block.h"
#include "Chunk.h"
#include "Renderer.h"
#include "ThreadPool.h"
#include "World.h"


class Game {
public:
    Game(const Game&) = delete;
    void operator=(const Game&) = delete;

    static Game& Get() {
        static Game instance;
        return instance;
    }

    void init();
    void main_loop();
    Camera m_camera;
    World m_world;
    Renderer m_renderer;
    float m_speed_adjust;
    Vector3 m_camera_speed;
    ThreadPool m_thread_pool = ThreadPool(6);
    const int RENDER_DISTANCE = 8;
    const int WORLD_SEED = 12321;
private:
    Game(){}
};
