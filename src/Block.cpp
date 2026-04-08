#include "Block.h"
#include "raylib.h"
#include "ResourceManager.h"

Block::Block(int material_type)
{
    m_material_type = material_type;
}

Block::Block()
{
    m_material_type = 0; //air
}









// Block::Block(Vector3 position, int material_type) {
//     m_position = position; //corner that has the smallest coordinates in x,y,z
//     m_size = {1, 1, 1};
//     m_position_middle = {
//         m_position.x + float(0.5 * m_size.x), m_position.y + float(0.5 * m_size.y), m_position.z + float(0.5 * m_size.z)
//     };
//     m_material_type = material_type;
// }
// Block::Block(Vector3 chunk_position, Vector3 position_in_chunk) {
//     m_position_in_chunk = position_in_chunk;
//     m_position.x = chunk_position.x + position_in_chunk.x;
//     m_position.y = chunk_position.y + position_in_chunk.y;
//     m_position.z = chunk_position.z + position_in_chunk.z;
//     m_size = {1, 1, 1};
//     m_position_middle = {
//         m_position.x + float(0.5 * m_size.x), m_position.y + float(0.5 * m_size.y), m_position.z + float(0.5 * m_size.z)
//     };
//     m_material_type = 1;//dirt
//
// }



// void Block::draw() const {
//     // DrawCubeV(m_position_middle, m_size, m_color);
//     switch (m_material_type) {
//         case 0:
//             break;
//         case 1:
//             DrawModel(ResourceManager::Get().DIRT_MODEL, m_position_middle, 1, WHITE);
//         default:
//             break;
//     }
//
// }


