#pragma once
#include <memory>

#include "Block.h"
#include "Chunk.h"
#include "Player.h"
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
    World m_world;
    Renderer m_renderer;
    Player m_player;
    ThreadPool m_thread_pool;
    const int RENDER_DISTANCE = 14;
    const int WORLD_SEED = 12521;
    bool m_enable_player_collision = true;
    bool m_is_debug_menu = false;
    bool m_is_escape_menu = false;
    bool m_window_should_close = false;
private:
    Game() : m_thread_pool(6)
    {}
    void run_imgui();
    void setup_imgui();
    void debug_menu();
    void escape_menu();
    void set_minecraft_style();
};
