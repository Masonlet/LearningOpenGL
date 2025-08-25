#pragma once

#include "vec2.hpp"
#include "vec3.hpp"
#include "vec4.hpp"

struct Grid {
	std::string name;
	unsigned int count{ 1 };
	float spacing{ 1.0f };
	Transform transform;
	Vec4 colour{ 1.0f };
	ColourMode colourMode{ ColourMode::Solid };
};