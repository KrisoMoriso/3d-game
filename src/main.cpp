#include "Game.h"
#include "../cmake-build-debug/_deps/raylib-src/src/raylib.h"

int main() {
    Game::Get().init();
    while (!WindowShouldClose()) {
        Game::Get().main_loop();
    }
    CloseWindow();
}
