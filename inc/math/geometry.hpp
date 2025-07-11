#pragma once

#include "vec2.hpp"
#include "vec3.hpp"
#include "vec4.hpp"

struct Triangle {
	Vec4 position;
	Vec3 rotation;
	Vec3 scale;

	Triangle();
};

struct Square {
	Vec4 position;
	Vec3 rotation;
	Vec2 size;

	Square();
};

struct Grid {
	unsigned int count;
	float spacing;
	Vec3 start;
	Vec3 rotation;
	Vec3 scale;

	Grid();
};