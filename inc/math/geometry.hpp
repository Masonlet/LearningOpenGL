#pragma once

#include "vec2.hpp"
#include "vec3.hpp"
#include "vec4.hpp"

struct Triangle {
	std::string name;
	Vec3 pos  { 0.0f };
	Vec3 rot  { 0.0f };
	Vec3 size{ 1.0f };
	Vec4 colour{ 1.0f };
	ColourMode colourMode{ ColourMode::Solid };
};

struct Square {
	std::string name;
	Vec4 pos { 0.0f };
	Vec3 rot { 0.0f };
	Vec2 size{ 1.0f };
	Vec4 colour{ 1.0f };
	ColourMode colourMode{ ColourMode::Solid };
};

struct Grid {
	std::string name;
	unsigned int count{ 1 };
	float spacing{ 1.0f };
	Vec3 start{ 0.0f };
	Vec3 rot{ 0.0f };
	Vec3 size{ 1.0f };
	Vec4 colour{ 1.0f };
	ColourMode colourMode{ ColourMode::Solid };
};