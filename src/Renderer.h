#pragma once
#include "raylib.h"


class Renderer {
public:
    Model m_cube_model;
    void update_mesh();
    void render();
    Renderer(){}
};
