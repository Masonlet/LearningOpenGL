#pragma once

#include "Vec3.hpp"
#include "Vec2.hpp"

struct Vertex {
	Vec3 pos;
	Vec3 norm;
	Vec3 col;
	Vec2 texCoord;
};
