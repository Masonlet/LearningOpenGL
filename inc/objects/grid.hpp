#pragma once

#include "math/vec4.hpp"
#include "math/transform.hpp"
#include "objects/colour.hpp"
#include <string>

struct Grid {
	std::string name;
	unsigned int count{ 1 };
	float spacing{ 1.0f };
	Transform transform;
	Vec4 colour{ 1.0f };
	ColourMode colourMode{ ColourMode::Solid };
};
