#include "Game.h"
#include "../cmake-build-debug/_deps/raylib-src/src/raylib.h"
#include "imgui/rlImGui.h"

int main() {
    Game::Get().init();
    while (!WindowShouldClose() and !Game::Get().m_window_should_close) {
        Game::Get().main_loop();
    }
    rlImGuiShutdown();
    CloseWindow();
}
