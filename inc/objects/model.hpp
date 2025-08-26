#pragma once

#include "math/vec2.hpp"
#include "math/vec3.hpp"
#include "math/vec4.hpp"
#include "math/mat4.hpp"
#include "objects/colour.hpp"
#include <string>

struct Model {
	std::string name;
	std::string meshPath;

	Transform transform;
	Vec4 colour{ 1.0f }, specular{ 1.0f, 1.0f, 1.0f, 32.0f };
	ColourMode colourMode{ ColourMode::Solid };
	bool isVisible{ true }, isLighted{ true }, useTextures{ false };

	static const unsigned int NUM_TEXTURES = 4;
	std::string textureNames[NUM_TEXTURES]{ "" };
	float textureMixRatio[NUM_TEXTURES]{ 1.0f, 0.0f, 0.0f, 0.0f };
	Vec2 textureTiling{ 1.0f, 1.0f };
};