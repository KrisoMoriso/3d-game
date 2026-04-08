#include "Chunk.h"

Chunk::Chunk(Vector3 chunk_id, Vector3 position) {
    m_chunk_id = chunk_id;
    m_position = position;
    m_blocks.resize(4096);
    for (int x = 0; x <= 15; ++x) {
        for (int y = 0; y <= 15; ++y) {
            for (int z = 0; z <= 15; ++z) {
                m_blocks[z + y * 16 + x*256] = Block(m_position, Vector3{float(x), float(y) , float(z)} );
            }
        }
    }
}

void Chunk::draw() {
    for (Block element: m_blocks) {
        element.draw();
    }
}


