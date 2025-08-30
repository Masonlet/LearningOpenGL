#pragma once

#include "starletmath/vec4.hpp"
#include "starletmath/transform.hpp"
#include "scene/objects/colour.hpp"
#include <string>

struct Grid {
	std::string name;
	unsigned int count{ 1 };
	float spacing{ 1.0f };
	Transform transform;
	Vec4 colour{ 1.0f };
	ColourMode colourMode{ ColourMode::Solid };
};
