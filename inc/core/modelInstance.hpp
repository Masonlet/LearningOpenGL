#pragma once

#include "math/vec3.hpp"
#include "math/vec4.hpp"
#include "math/mat4.hpp"
#include "core/colour.hpp"
#include <string>

struct ModelInstance {
	std::string name;
	std::string meshPath;
	Vec3 position{ 0.0f }, rotation{ 0.0f }, scale{ 1.0f };
	Mat4 modelMatrix;
	Vec4 colour{ 1.0f }, specular{ 1.0f, 1.0f, 1.0f, 32.0f };
	ColourMode colourMode{ ColourMode::Solid };
	bool isVisible{ false }, isLighted{ true }, useTextures{ false };
};