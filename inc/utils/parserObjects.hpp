#pragma once

#include "math/vec3.hpp"
#include "math/vec4.hpp"
#include "core/colour.hpp"
#include "math/geometry.hpp"

#include <string>
#include <vector>

struct ParsedModel {
	std::string meshName;
	std::string path;
	Vec3 position;
	Vec3 rotation;
	Vec3 scale;
	Vec4 colour;
	ColourMode colourMode;
};
struct ParsedLight {
	std::string name;
	Vec3 position;
	Vec4 diffuse;
	Vec4 atten;
	Vec4 direction;

	unsigned int type;
	Vec3 param1;
	Vec4 param2;
};
struct ParsedCamera {
	unsigned int index;
	Vec3 position;
	float yaw, pitch;
	float fov;
	float nearPlane, farPlane;
};

struct ParsedTriangle {
	std::string meshName;
	Triangle transform;
	Vec4 colour;
	ColourMode colourMode;
};
struct ParsedGrid {
	std::string meshName;
	Grid layout;
	Vec4 colour;
	ColourMode colourMode{ ColourMode::Solid };
};

struct ParsedMaze {
	std::string mazeName;
	std::string floorType;
	std::string wallType;
	std::string layoutName;
	std::vector<std::vector<bool>> layout;
};
