#pragma once

#include "math/vec3.hpp"
#include "math/vec4.hpp"
#include "core/colour.hpp"
#include "math/geometry.hpp"

#include <string>
#include <vector>

struct ParsedModel {
	std::string meshName, path;
	Vec3 position{0.0f}, rotation{0.0f}, scale{1.0f};
	Vec4 colour{1.0f}, specular{1.0f, 1.0f, 1.0f, 32.0f};
	ColourMode colourMode{ColourMode::Solid};
};
struct ParsedLight {
	std::string name;
	Vec3 position{0.0f};
	Vec4 diffuse{1.0f}, atten{1.0f, 0.0f, 0.0f, 10000.0f}, direction{0.0f, -1.0f, 0.0f, 0.0f};
	unsigned int param1Type{ 1 };
	Vec3 param1Direction{ 0.0f };
	Vec4 param2{1.0f};
};
struct ParsedCamera {
	unsigned int type{ 0 };

	Vec3 position{0.0f};
	float yaw{0.0f}, pitch{0.0f};
	float fov{75.0f}, nearPlane{0.1f}, farPlane{10000.0f};
	float speed{200.0f}, moveDistance{1.0f};
};

struct ParsedTriangle {
	std::string meshName;
	Triangle transform;
	Vec4 colour{1.0f};
	ColourMode colourMode{ColourMode::Solid};
};
struct ParsedGrid {
	std::string meshName;
	Grid layout;
	Vec4 colour{1.0f};
	ColourMode colourMode{ColourMode::Solid};
};

struct ParsedMaze {
	std::string mazeName, layoutName;
	Vec3 pos{ 0.0f }, rot{ 0.0f };
	float spacing{ 0.0f }, wallRot{ 0.0f };
	unsigned int wallHeight{ 0 };
	bool hasRoof{ false };
	std::vector<std::vector<bool>> layout{};
	std::string floorType1, floorType2, floorType3, floorType4, floorType5, floorType6, floorWallType;
	std::string wallType1, wallType2, wallType3, wallType4, wallType5, wallType6;
	std::string entranceType, exitType, exteriorWallType;
};
