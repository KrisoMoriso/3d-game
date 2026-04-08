#pragma once
#include <memory>

#include "Block.h"
#include "Chunk.h"


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
    Chunk m_chunk_1;
    Chunk m_chunk_2;
    Camera m_camera;
    float m_speed_adjust;
    Vector3 m_camera_speed;
private:
    Game(){}
};
