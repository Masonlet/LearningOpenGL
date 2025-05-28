#pragma once

#include "Vec3.hpp"
#include "Vec2.hpp"


/* 
Vertex
* Vertex structure as it is in the SHADER (on the GPU)
* This is also called the 'vertex layout'
* If the shader vertex layout changed, 
* this vertex structure will also need to change (usually) 
*/
struct Vertex {
	Vec3 pos; // float x, float y, float z
	Vec3 col; // float r, float g, float b
	Vec3 norm; //float nx, float ny, float nz
};
