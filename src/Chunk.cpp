#include "Chunk.h"

Chunk::Chunk() {
    m_blocks.resize(4096);
    for (int x = 0; x <= 15; ++x) {
        for (int y = 0; y <= 15; ++y) {
            for (int z = 0; z <= 15; ++z) {
                m_blocks[z + y * 16 + x*256] = Block( 0 );
            }
        }
    }
}




