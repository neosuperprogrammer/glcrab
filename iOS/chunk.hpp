//
//  chunk.hpp
//  templateApp
//
//  Created by Sangwook Nam on 6/28/14.
//  Copyright (c) 2014 ROm. All rights reserved.
//

#ifndef templateApp_chunk_hpp
#define templateApp_chunk_hpp

#include "glm.hpp"
#include "gfx.h"
#include "noise.hpp"

#define CX 16
#define CY 16
#define CZ 16

#define SEALEVEL 4

typedef glm::detail::tvec4<GLbyte, glm::defaultp> byte4;

//struct byte4
//{
//    typedef signed char byte;
//	byte x;
//	byte y;
//	byte z;
//	byte t;
//
//    byte4() {};
//    byte4(byte x_, byte y_, byte z_, byte t_) : x(x_), y(y_), z(z_), t(t_) {}
//};

static const int transparent[16] = {2, 0, 0, 0, 1, 0, 0, 0, 3, 4, 0, 0, 0, 0, 0, 0};
static const char *blocknames[16] = {
	"air", "dirt", "topsoil", "grass", "leaves", "wood", "stone", "sand",
	"water", "glass", "brick", "ore", "woodrings", "white", "black", "x-y"
};

struct chunk {
//    uint8_t blk[CX][CY][CZ];
//    GLuint vbo;
//    int elements;
//    bool changed;
    
    uint8_t blk[CX][CY][CZ];
	struct chunk *left, *right, *below, *above, *front, *back;
	int slot;
	GLuint vbo;
	int elements;
	time_t lastused;
	bool changed;
	bool noised;
	bool initialized;
	int ax;
	int ay;
	int az;
    
    chunk() {
        memset(blk, 0, sizeof blk);
        //        vertices = 0;
        changed = true;
        glGenBuffers(1, &vbo);
    }
    
    ~chunk() {
        glDeleteBuffers(1, &vbo);
    }
    
    uint8_t get(int x, int y, int z) {
        return blk[x][y][z];
    }
    
    void set(int x, int y, int z, uint8_t type) {
        blk[x][y][z] = type;
        changed = true;
    }
    
    bool isblocked(int x1, int y1, int z1, int x2, int y2, int z2) {
		// Invisible blocks are always "blocked"
		if(!blk[x1][y1][z1])
			return true;
        
		// Leaves do not block any other block, including themselves
		if(transparent[get(x2, y2, z2)] == 1)
			return false;
        
		// Non-transparent blocks always block line of sight
		if(!transparent[get(x2, y2, z2)])
			return true;
        
		// Otherwise, LOS is only blocked by blocks if the same transparency type
		return transparent[get(x2, y2, z2)] == transparent[blk[x1][y1][z1]];
	}
    
    void random(void) {
        for(int x = 0; x < CX; x++) {
			for(int y = 0; y < CY; y++) {
				for(int z = 0; z < CZ; z++) {
                    set(x, y, z, (x*y*z) % 512);
                }
            }
        }
    }
    
	void update() {
		byte4 vertex[CX * CY * CZ * 6 * 6];
		int i = 0;
        
		for(int x = 0; x < CX; x++) {
			for(int y = 0; y < CY; y++) {
                bool visible = false;
                
				for(int z = 0; z < CZ; z++) {
                    
                    // Empty block?
                    if(!blk[x][y][z]) {
                        visible = false;
                        continue;
                    }
                    
					uint8_t side = blk[x][y][z];
                    
//                    // View from negative x
//                    vertex[i++] = byte4(x, y, z, blk[x][y][z]);
//                    vertex[i++] = byte4(x, y, z + 1, blk[x][y][z]);
//                    vertex[i++] = byte4(x, y + 1, z, blk[x][y][z]);
//                    vertex[i++] = byte4(x, y + 1, z, blk[x][y][z]);
//                    vertex[i++] = byte4(x, y, z + 1, blk[x][y][z]);
//                    vertex[i++] = byte4(x, y + 1, z + 1, blk[x][y][z]);
                    
                    if(visible && blk[x][y][z] == blk[x - 1][y][z]) {
                        vertex[i - 5] = byte4(x,     y,     z + 1, blk[x][y][z]);
                        vertex[i - 2] = byte4(x,     y,     z + 1, blk[x][y][z]);
                        vertex[i - 1] = byte4(x,     y + 1, z + 1, blk[x][y][z]);
                    }
                    
                    else {
                        
                        // View from negative x, only draw if there is no block in front of it
                        if(x > 0 && !blk[x - 1][y][z]) {
                            vertex[i++] = byte4(x,     y,     z,     blk[x][y][z]);
                            vertex[i++] = byte4(x,     y,     z + 1, blk[x][y][z]);
                            vertex[i++] = byte4(x,     y + 1, z,     blk[x][y][z]);
                            vertex[i++] = byte4(x,     y + 1, z,     blk[x][y][z]);
                            vertex[i++] = byte4(x,     y,     z + 1, blk[x][y][z]);
                            vertex[i++] = byte4(x,     y + 1, z + 1, blk[x][y][z]);
                            visible = true;
                        }
                        else {
                            visible = false;
                        }
                    }
                    
                    if(visible && blk[x][y][z] == blk[x - 1][y][z]) {
                        vertex[i - 5] = byte4(x + 1,     y,     z + 1, blk[x][y][z]);
                        vertex[i - 2] = byte4(x + 1,     y,     z + 1, blk[x][y][z]);
                        vertex[i - 1] = byte4(x + 1,     y + 1, z + 1, blk[x][y][z]);
                    }
                    else {
                        
                        // View from positive x
                        if(x == CX - 1 || (x < CX - 1 && !blk[x + 1][y][z])) {
                            vertex[i++] = byte4(x + 1, y, z, blk[x][y][z]);
                            vertex[i++] = byte4(x + 1, y + 1, z, blk[x][y][z]);
                            vertex[i++] = byte4(x + 1, y, z + 1, blk[x][y][z]);
                            vertex[i++] = byte4(x + 1, y + 1, z, blk[x][y][z]);
                            vertex[i++] = byte4(x + 1, y + 1, z + 1, blk[x][y][z]);
                            vertex[i++] = byte4(x + 1, y, z + 1, blk[x][y][z]);
                            visible = true;
                        }
                        else {
                            visible = false;
                        }
                    }
                    // View from negative y
//                    vertex[i++] = byte4(x, y, z, blk[x][y][z]);
//                    vertex[i++] = byte4(x + 1, y, z, blk[x][y][z]);
//                    vertex[i++] = byte4(x, y, z + 1, blk[x][y][z]);
//                    vertex[i++] = byte4(x + 1, y, z, blk[x][y][z]);
//                    vertex[i++] = byte4(x + 1, y, z + 1, blk[x][y][z]);
//                    vertex[i++] = byte4(x, y, z + 1, blk[x][y][z]);
                    
                    // View from negative y, only draw if there is no block in front of it
                    if(y > 0 && !blk[x][y-1][z]) {
                        vertex[i++] = byte4(x, y, z, blk[x][y][z]);
                        vertex[i++] = byte4(x + 1, y, z, blk[x][y][z]);
                        vertex[i++] = byte4(x, y, z + 1, blk[x][y][z]);
                        vertex[i++] = byte4(x + 1, y, z, blk[x][y][z]);
                        vertex[i++] = byte4(x + 1, y, z + 1, blk[x][y][z]);
                        vertex[i++] = byte4(x, y, z + 1, blk[x][y][z]);
                    }
                    
                    
                    // View from positive y
                    if(y == CY - 1 || (y < CY - 1 && !blk[x][y+1][z])) {
                        vertex[i++] = byte4(x, y + 1, z, blk[x][y][z]);
                        vertex[i++] = byte4(x, y + 1, z + 1, blk[x][y][z]);
                        vertex[i++] = byte4(x + 1, y + 1, z, blk[x][y][z]);
                        vertex[i++] = byte4(x + 1, y + 1, z, blk[x][y][z]);
                        vertex[i++] = byte4(x, y + 1, z + 1, blk[x][y][z]);
                        vertex[i++] = byte4(x + 1, y + 1, z + 1, blk[x][y][z]);
                    }
                    
                    // View from negative z
//                    vertex[i++] = byte4(x, y, z, side);
//                    vertex[i++] = byte4(x, y + 1, z, side);
//                    vertex[i++] = byte4(x + 1, y, z, side);
//                    vertex[i++] = byte4(x, y + 1, z, side);
//                    vertex[i++] = byte4(x + 1, y + 1, z, side);
//                    vertex[i++] = byte4(x + 1, y, z, side);
                    
                    // View from negative z, only draw if there is no block in front of it
                    if(z > 0 && !blk[x][y][z-1]) {
                        vertex[i++] = byte4(x, y, z, side);
                        vertex[i++] = byte4(x, y + 1, z, side);
                        vertex[i++] = byte4(x + 1, y, z, side);
                        vertex[i++] = byte4(x, y + 1, z, side);
                        vertex[i++] = byte4(x + 1, y + 1, z, side);
                        vertex[i++] = byte4(x + 1, y, z, side);
                    }
                    
                    // View from positive z
                    if(z == CZ - 1 || (z < CZ - 1 && !blk[x][y][z+1])) {
                    vertex[i++] = byte4(x, y, z + 1, side);
                    vertex[i++] = byte4(x + 1, y, z + 1, side);
                    vertex[i++] = byte4(x, y + 1, z + 1, side);
                    vertex[i++] = byte4(x, y + 1, z + 1, side);
                    vertex[i++] = byte4(x + 1, y, z + 1, side);
                    vertex[i++] = byte4(x + 1, y + 1, z + 1, side);
                    }
                    
                    
				}
			}
		}
        
        
		changed = false;
		elements = i;
        
		// If this chunk is empty, no need to allocate a chunk slot.
		if(!elements)
			return;
        
        
		glBindBuffer(GL_ARRAY_BUFFER, vbo);
		glBufferData(GL_ARRAY_BUFFER, elements * sizeof(vertex[0]), vertex, GL_STATIC_DRAW);
	}
    
    //	void update() {
    //		byte44 vertex[CX * CY * CZ * 18];
    //		int i = 0;
    //		int merged = 0;
    //		bool vis = false;;
    //
    //		// View from negative x
    //
    //		for(int x = CX - 1; x >= 0; x--) {
    //			for(int y = 0; y < CY; y++) {
    //				for(int z = 0; z < CZ; z++) {
    //					// Line of sight blocked?
    //					if(isblocked(x, y, z, x - 1, y, z)) {
    //						vis = false;
    //						continue;
    //					}
    //
    //					uint8_t top = blk[x][y][z];
    //					uint8_t bottom = blk[x][y][z];
    //					uint8_t side = blk[x][y][z];
    //
    //					// Grass block has dirt sides and bottom
    //					if(top == 3) {
    //						bottom = 1;
    //						side = 2;
    //                        // Wood blocks have rings on top and bottom
    //					} else if(top == 5) {
    //						top = bottom = 12;
    //					}
    //
    //					// Same block as previous one? Extend it.
    //					if(vis && z != 0 && blk[x][y][z] == blk[x][y][z - 1]) {
    //						vertex[i - 5] = byte44(x, y, z + 1, side);
    //						vertex[i - 2] = byte44(x, y, z + 1, side);
    //						vertex[i - 1] = byte44(x, y + 1, z + 1, side);
    //						merged++;
    //                        // Otherwise, add a new quad.
    //					} else {
    //						vertex[i++] = byte44(x, y, z, side);
    //						vertex[i++] = byte44(x, y, z + 1, side);
    //						vertex[i++] = byte44(x, y + 1, z, side);
    //						vertex[i++] = byte44(x, y + 1, z, side);
    //						vertex[i++] = byte44(x, y, z + 1, side);
    //						vertex[i++] = byte44(x, y + 1, z + 1, side);
    //					}
    //
    //					vis = true;
    //				}
    //			}
    //		}
    //
    //		// View from positive x
    //
    //		for(int x = 0; x < CX; x++) {
    //			for(int y = 0; y < CY; y++) {
    //				for(int z = 0; z < CZ; z++) {
    //					if(isblocked(x, y, z, x + 1, y, z)) {
    //						vis = false;
    //						continue;
    //					}
    //
    //					uint8_t top = blk[x][y][z];
    //					uint8_t bottom = blk[x][y][z];
    //					uint8_t side = blk[x][y][z];
    //
    //					if(top == 3) {
    //						bottom = 1;
    //						side = 2;
    //					} else if(top == 5) {
    //						top = bottom = 12;
    //					}
    //
    //					if(vis && z != 0 && blk[x][y][z] == blk[x][y][z - 1]) {
    //						vertex[i - 4] = byte44(x + 1, y, z + 1, side);
    //						vertex[i - 2] = byte44(x + 1, y + 1, z + 1, side);
    //						vertex[i - 1] = byte44(x + 1, y, z + 1, side);
    //						merged++;
    //					} else {
    //						vertex[i++] = byte44(x + 1, y, z, side);
    //						vertex[i++] = byte44(x + 1, y + 1, z, side);
    //						vertex[i++] = byte44(x + 1, y, z + 1, side);
    //						vertex[i++] = byte44(x + 1, y + 1, z, side);
    //						vertex[i++] = byte44(x + 1, y + 1, z + 1, side);
    //						vertex[i++] = byte44(x + 1, y, z + 1, side);
    //					}
    //					vis = true;
    //				}
    //			}
    //		}
    //
    //		// View from negative y
    //
    //		for(int x = 0; x < CX; x++) {
    //			for(int y = CY - 1; y >= 0; y--) {
    //				for(int z = 0; z < CZ; z++) {
    //					if(isblocked(x, y, z, x, y - 1, z)) {
    //						vis = false;
    //						continue;
    //					}
    //
    //					uint8_t top = blk[x][y][z];
    //					uint8_t bottom = blk[x][y][z];
    //
    //					if(top == 3) {
    //						bottom = 1;
    //					} else if(top == 5) {
    //						top = bottom = 12;
    //					}
    //
    //					if(vis && z != 0 && blk[x][y][z] == blk[x][y][z - 1]) {
    //						vertex[i - 4] = byte44(x, y, z + 1, bottom + 128);
    //						vertex[i - 2] = byte44(x + 1, y, z + 1, bottom + 128);
    //						vertex[i - 1] = byte44(x, y, z + 1, bottom + 128);
    //						merged++;
    //					} else {
    //						vertex[i++] = byte44(x, y, z, bottom + 128);
    //						vertex[i++] = byte44(x + 1, y, z, bottom + 128);
    //						vertex[i++] = byte44(x, y, z + 1, bottom + 128);
    //						vertex[i++] = byte44(x + 1, y, z, bottom + 128);
    //						vertex[i++] = byte44(x + 1, y, z + 1, bottom + 128);
    //						vertex[i++] = byte44(x, y, z + 1, bottom + 128);
    //					}
    //					vis = true;
    //				}
    //			}
    //		}
    //
    //		// View from positive y
    //
    //		for(int x = 0; x < CX; x++) {
    //			for(int y = 0; y < CY; y++) {
    //				for(int z = 0; z < CZ; z++) {
    //					if(isblocked(x, y, z, x, y + 1, z)) {
    //						vis = false;
    //						continue;
    //					}
    //
    //					uint8_t top = blk[x][y][z];
    //					uint8_t bottom = blk[x][y][z];
    //
    //					if(top == 3) {
    //						bottom = 1;
    //					} else if(top == 5) {
    //						top = bottom = 12;
    //					}
    //
    //					if(vis && z != 0 && blk[x][y][z] == blk[x][y][z - 1]) {
    //						vertex[i - 5] = byte44(x, y + 1, z + 1, top + 128);
    //						vertex[i - 2] = byte44(x, y + 1, z + 1, top + 128);
    //						vertex[i - 1] = byte44(x + 1, y + 1, z + 1, top + 128);
    //						merged++;
    //					} else {
    //						vertex[i++] = byte44(x, y + 1, z, top + 128);
    //						vertex[i++] = byte44(x, y + 1, z + 1, top + 128);
    //						vertex[i++] = byte44(x + 1, y + 1, z, top + 128);
    //						vertex[i++] = byte44(x + 1, y + 1, z, top + 128);
    //						vertex[i++] = byte44(x, y + 1, z + 1, top + 128);
    //						vertex[i++] = byte44(x + 1, y + 1, z + 1, top + 128);
    //					}
    //					vis = true;
    //				}
    //			}
    //		}
    //
    //		// View from negative z
    //
    //		for(int x = 0; x < CX; x++) {
    //			for(int z = CZ - 1; z >= 0; z--) {
    //				for(int y = 0; y < CY; y++) {
    //					if(isblocked(x, y, z, x, y, z - 1)) {
    //						vis = false;
    //						continue;
    //					}
    //
    //					uint8_t top = blk[x][y][z];
    //					uint8_t bottom = blk[x][y][z];
    //					uint8_t side = blk[x][y][z];
    //
    //					if(top == 3) {
    //						bottom = 1;
    //						side = 2;
    //					} else if(top == 5) {
    //						top = bottom = 12;
    //					}
    //
    //					if(vis && y != 0 && blk[x][y][z] == blk[x][y - 1][z]) {
    //						vertex[i - 5] = byte44(x, y + 1, z, side);
    //						vertex[i - 3] = byte44(x, y + 1, z, side);
    //						vertex[i - 2] = byte44(x + 1, y + 1, z, side);
    //						merged++;
    //					} else {
    //						vertex[i++] = byte44(x, y, z, side);
    //						vertex[i++] = byte44(x, y + 1, z, side);
    //						vertex[i++] = byte44(x + 1, y, z, side);
    //						vertex[i++] = byte44(x, y + 1, z, side);
    //						vertex[i++] = byte44(x + 1, y + 1, z, side);
    //						vertex[i++] = byte44(x + 1, y, z, side);
    //					}
    //					vis = true;
    //				}
    //			}
    //		}
    //
    //		// View from positive z
    //
    //		for(int x = 0; x < CX; x++) {
    //			for(int z = 0; z < CZ; z++) {
    //				for(int y = 0; y < CY; y++) {
    //					if(isblocked(x, y, z, x, y, z + 1)) {
    //						vis = false;
    //						continue;
    //					}
    //
    //					uint8_t top = blk[x][y][z];
    //					uint8_t bottom = blk[x][y][z];
    //					uint8_t side = blk[x][y][z];
    //
    //					if(top == 3) {
    //						bottom = 1;
    //						side = 2;
    //					} else if(top == 5) {
    //						top = bottom = 12;
    //					}
    //
    //					if(vis && y != 0 && blk[x][y][z] == blk[x][y - 1][z]) {
    //						vertex[i - 4] = byte44(x, y + 1, z + 1, side);
    //						vertex[i - 3] = byte44(x, y + 1, z + 1, side);
    //						vertex[i - 1] = byte44(x + 1, y + 1, z + 1, side);
    //						merged++;
    //					} else {
    //						vertex[i++] = byte44(x, y, z + 1, side);
    //						vertex[i++] = byte44(x + 1, y, z + 1, side);
    //						vertex[i++] = byte44(x, y + 1, z + 1, side);
    //						vertex[i++] = byte44(x, y + 1, z + 1, side);
    //						vertex[i++] = byte44(x + 1, y, z + 1, side);
    //						vertex[i++] = byte44(x + 1, y + 1, z + 1, side);
    //					}
    //					vis = true;
    //				}
    //			}
    //		}
    //
    //		changed = false;
    //		elements = i;
    //
    //		// If this chunk is empty, no need to allocate a chunk slot.
    //		if(!elements)
    //			return;
    //
    //		// If we don't have an active slot, find one
    ////		if(chunk_slot[slot] != this) {
    ////			int lru = 0;
    ////			for(int i = 0; i < CHUNKSLOTS; i++) {
    ////				// If there is an empty slot, use it
    ////				if(!chunk_slot[i]) {
    ////					lru = i;
    ////					break;
    ////				}
    ////				// Otherwise try to find the least recently used slot
    ////				if(chunk_slot[i]->lastused < chunk_slot[lru]->lastused)
    ////					lru = i;
    ////			}
    ////            
    ////			// If the slot is empty, create a new VBO
    ////			if(!chunk_slot[lru]) {
    ////				glGenBuffers(1, &vbo);
    ////                // Otherwise, steal it from the previous slot owner
    ////			} else {
    ////				vbo = chunk_slot[lru]->vbo;
    ////				chunk_slot[lru]->changed = true;
    ////			}
    ////            
    ////			slot = lru;
    ////			chunk_slot[slot] = this;
    ////		}
    //        
    //		// Upload vertices
    //        
    //		glBindBuffer(GL_ARRAY_BUFFER, vbo);
    //		glBufferData(GL_ARRAY_BUFFER, i * sizeof *vertex, vertex, GL_STATIC_DRAW);
    //	}
    
    void render(GLuint attribute_coord) {
        if(changed)
            update();
        // Render the VBO here
        // If this chunk is empty, we don't need to draw anything.
        if(!elements)
            return;
        
        glEnable(GL_CULL_FACE);
        glEnable(GL_DEPTH_TEST);
        
        glBindBuffer(GL_ARRAY_BUFFER, vbo);
        glVertexAttribPointer(attribute_coord, 4, GL_BYTE, GL_FALSE, 0, 0);
        glDrawArrays(GL_TRIANGLES, 0, elements);
    }
    
    static float noise2d(float x, float y, int seed, int octaves, float persistence) {
		float sum = 0;
		float strength = 1.0;
		float scale = 1.0;
        
		for(int i = 0; i < octaves; i++) {
			sum += strength * glm::simplex(glm::vec2(x, y) * scale);
			scale *= 2.0;
			strength *= persistence;
		}
        
		return sum;
	}
    
	static float noise3d_abs(float x, float y, float z, int seed, int octaves, float persistence) {
		float sum = 0;
		float strength = 1.0;
		float scale = 1.0;
        
		for(int i = 0; i < octaves; i++) {
			sum += strength * fabs(glm::simplex(glm::vec3(x, y, z) * scale));
			scale *= 2.0;
			strength *= persistence;
		}
        
		return sum;
	}
    
	void noise(int seed) {
		if(noised)
			return;
		else
			noised = true;
        
		for(int x = 0; x < CX; x++) {
			for(int z = 0; z < CZ; z++) {
				// Land height
				float n = noise2d((x + ax * CX) / 256.0, (z + az * CZ) / 256.0, seed, 5, 0.8) * 4;
				int h = n * 2;
				int y = 0;
                
				// Land blocks
				for(y = 0; y < CY; y++) {
					// Are we above "ground" level?
					if(y + ay * CY >= h) {
						// If we are not yet up to sea level, fill with water blocks
						if(y + ay * CY < SEALEVEL) {
							blk[x][y][z] = 8;
							continue;
                            // Otherwise, we are in the air
						} else {
							// A tree!
							if(get(x, y - 1, z) == 3 && (rand() & 0xff) == 0) {
								// Trunk
								h = (rand() & 0x3) + 3;
								for(int i = 0; i < h; i++)
									set(x, y + i, z, 5);
                                
								// Leaves
								for(int ix = -3; ix <= 3; ix++) {
									for(int iy = -3; iy <= 3; iy++) {
										for(int iz = -3; iz <= 3; iz++) {
											if(ix * ix + iy * iy + iz * iz < 8 + (rand() & 1) && !get(x + ix, y + h + iy, z + iz))
												set(x + ix, y + h + iy, z + iz, 4);
										}
									}
								}
							}
							break;
						}
					}
                    
					// Random value used to determine land type
					float r = noise3d_abs((x + ax * CX) / 16.0, (y + ay * CY) / 16.0, (z + az * CZ) / 16.0, -seed, 2, 1);
                    
					// Sand layer
					if(n + r * 5 < 4)
						blk[x][y][z] = 7;
					// Dirt layer, but use grass blocks for the top
					else if(n + r * 5 < 8)
						blk[x][y][z] = (h < SEALEVEL || y + ay * CY < h - 1) ? 1 : 3;
					// Rock layer
					else if(r < 1.25)
						blk[x][y][z] = 6;
					// Sometimes, ores!
					else
						blk[x][y][z] = 11;
				}
			}
		}
		changed = true;
	}
};


#define SCX 8
#define SCY 8
#define SCZ 8

struct superchunk {
    chunk *c[SCX][SCY][SCZ];
    
    superchunk() {
        memset(c, 0, sizeof c);
    }
    
    ~superchunk() {
        for(int x = 0; x < SCX; x++)
            for(int y = 0; y < SCX; y++)
                for(int z = 0; z < SCX; z++)
                    delete c[x][y][z];
    }
    
    void random(void) {
        for(int x = 0; x < CX * SCX; x++) {
			for(int y = 0; y < CY * SCY; y++) {
				for(int z = 0; z < CZ * SCZ; z++) {
                    set(x, y, z, (x*y*z) % 512);
                }
            }
        }
    }
    
    void noise(int seed) {
        for(int x = 0; x < SCX; x++) {
            for(int y = 0; y < SCY; y++) {
                for(int z = 0; z < SCZ; z++) {
                    set(x, y, z, (x*y*z) % 512);
                }
            }
        }
    }
    
    uint8_t get(int x, int y, int z) {
        int cx = x / CX;
        int cy = y / CY;
        int cz = z / CZ;
        
        x %= CX;
        y %= CY;
        z %= CZ;
        
        if(!c[cx][cy][cz])
            return 0;
        else
            return c[cx][cy][cz]->get(x, y, z);
    }
    
    void set(int x, int y, int z, uint8_t type) {
        int cx = x / CX;
        int cy = y / CY;
        int cz = z / CZ;
        
        x %= CX;
        y %= CY;
        z %= CZ;
        
        if(!c[cx][cy][cz])
            c[cx][cy][cz] = new chunk();
        
        c[cx][cy][cz]->set(x, y, z, type);
    }
    
    void render(PROGRAM *program) {
        
        char attribute, uniform;
		
		uniform = PROGRAM_get_uniform_location( program,
                                               ( char * )"mvp" );
		
		attribute = PROGRAM_get_vertex_attrib_location( program,
                                                       ( char * )"coord" );
        
		glEnableVertexAttribArray( attribute );
        
        
        for(int x = 0; x < SCX; x++)
            for(int y = 0; y < SCY; y++)
                for(int z = 0; z < SCZ; z++)
                    if(c[x][y][z]) {
                        
                        GFX_push_matrix();
                        GFX_translate(x * CX, y * CY, z * CZ);

                        glUniformMatrix4fv( uniform,
                                           1 /* How many 4x4 matrix */,
                                           GL_FALSE /* Transpose the matrix? */,
                                           ( float * )GFX_get_modelview_projection_matrix() );
                        
                        GFX_pop_matrix();
                        
//                        glm::mat4 model = glm::translate(glm::mat4(1), glm::vec3(x * CX, y * CY, z * CZ));
                        // calculate the full MVP matrix here and pass it to the vertex shader
                        c[x][y][z]->render(attribute);
                    }
    }
};


#endif
