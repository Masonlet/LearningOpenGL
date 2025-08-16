#pragma once

#include "math/vec3.hpp"
#include "math/vec4.hpp"
#include "math/mat4.hpp"
#include "core/colour.hpp"
#include <string>

struct ModelInstance {
	Vec4 position;
	Vec3 rotation;
	Vec3 scale;
	Vec4 colour, specular;

	Mat4 modelMatrix;

	std::string path;
	ColourMode colourMode = ColourMode::Solid;
};