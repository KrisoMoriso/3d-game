#pragma once
#include <vector>

#include "raylib.h"


class Renderer {
public:
    Model m_cube_model;
    const float m_face_vertices[6][12]{
        // Front face (Z+)
        { 0,0,1,  1,0,1,  1,1,1,  0,1,1 },
        // Back face (Z-)
        { 1,0,0,  0,0,0,  0,1,0,  1,1,0 },
        // Left face (X-)
        { 0,0,0,  0,0,1,  0,1,1,  0,1,0 },
        // Right face (X+)
        { 1,0,1,  1,0,0,  1,1,0,  1,1,1 },
        // Top face (Y+)
        { 0,1,1,  1,1,1,  1,1,0,  0,1,0 },
        // Bottom face (Y-)
        { 0,0,0,  1,0,0,  1,0,1,  0,0,1 }
    };
    const unsigned short m_face_indices[6]
    {
        0, 1, 2,  // First triangle
        0, 2, 3   // Second triangle
    };
    const float m_face_UVs[8]
    {
        0.0f, 0.0f,  // Bottom-Left
    1.0f, 0.0f,  // Bottom-Right
    1.0f, 1.0f,  // Top-Right
    0.0f, 1.0f   // Top-Left
    };
    void update_mesh();
    void add_face(int face_id, int x, int y, int z,
                    std::vector<float>& vertices,
                    std::vector<float>& texcoords,
                    std::vector<unsigned short>& indices,
                    int& indice_counter);
    void render();

    Renderer(){}

};
