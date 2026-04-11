#include "Chunk.h"

#include "World.h"

Chunk::Chunk() {
    m_blocks.resize(4096);
    for (int x = 0; x <= 15; ++x) {
        for (int y = 0; y <= 15; ++y) {
            for (int z = 0; z <= 15; ++z) {
                m_blocks[z + y * 16 + x*256] = Block( World::BLOCK_MATERIALS::STONE );
            }
        }
    }
}

unsigned short Chunk::getBlockMaterial(Vector3 position){

    return m_blocks[position.x*256 + position.y*16 + position.z].m_material_type;

}

void Chunk::setBlock(int x, int y, int z, unsigned short material){
    m_blocks[z + y * 16 + x*256] = Block( material );
}

unsigned short Chunk::getBlockMaterial(int x, int y, int z){
    return m_blocks[z + y * 16 + x*256].m_material_type;
}




