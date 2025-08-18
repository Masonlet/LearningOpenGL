#pragma once

#include "math/vec2.hpp"
#include "math/vec3.hpp"
#include "math/vec4.hpp"

/* 
Vertex
* Vertex structure as it is in the SHADER (on the GPU)
* This is also called the 'vertex layout'
* If the shader vertex layout changed, 
* this vertex structure will also need to change (usually) 
*/
struct Vertex {
	Vec4 pos{ 0.0f }; // float x, float y, float z
	Vec4 col{ 1.0f }; // float r, float g, float b, float a
	Vec3 norm{ 0.0f }; //float nx, float ny, float nz
	Vec2 texCoord{ 0.0f }; // float u, float v
};
