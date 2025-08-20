#pragma once

#include "math/vec3.hpp"
#include "math/vec4.hpp"
#include "core/colour.hpp"
#include "math/geometry.hpp"

#include <string>
#include <vector>

struct ParsedModel {
	std::string name, path;
	bool isVisible{true}, isLighted{false};
	Vec3 position{0.0f}, rotation{0.0f}, scale{1.0f};
	Vec4 colour{1.0f}, specular{1.0f, 1.0f, 1.0f, 32.0f};
	ColourMode colourMode{ColourMode::Solid};
};
struct ParsedLight {
	std::string name;
	bool isEnabled;
	Vec3 position{0.0f};
	Vec4 diffuse{1.0f}, atten{1.0f, 0.0f, 0.0f, 10000.0f}, direction{0.0f, -1.0f, 0.0f, 0.0f};
	unsigned int param1Type{ 1 };
	Vec3 param1Direction{ 0.0f };
};
struct ParsedCamera {
	std::string name;
	unsigned int type{ 0 };
	bool isEnabled;
	Vec3 position{0.0f};
	float yaw{0.0f}, pitch{0.0f};
	float fov{75.0f}, nearPlane{0.1f}, farPlane{10000.0f};
	float speed{200.0f}, moveDistance{1.0f};
};

struct ParsedTriangle {
	std::string name;
	Triangle transform;
	Vec4 colour{1.0f};
	ColourMode colourMode{ColourMode::Solid};
};
struct ParsedGrid {
	std::string name;
	Grid layout;
	Vec4 colour{1.0f};
	ColourMode colourMode{ColourMode::Solid};
};

struct ParsedMaze {
	std::string mazeName, layoutName;
	Vec3 pos{ 0.0f }, rot{ 0.0f }, baseRot{ 0.0f };
	float spacing{ 0.0f };
	unsigned int wallHeight{ 0 };
	bool hasRoof{ false };
	std::vector<std::vector<bool>> layout{};
	std::string floorType1, floorType2, floorType3, floorType4, floorType5, floorType6, floorWallType;
	std::string wallType1, wallType2, wallType3, wallType4, wallType5, wallType6;
	std::string entranceType, exitType, exteriorWallType;
};

struct ParsedTexture {
	std::string modelName;
  std::string textureFile;
	unsigned int textureNum;
	float mix{ 1.0f };
	Vec2 tiling{ 1.0f, 1.0f };
};
struct ParsedTextureCube {
	std::string modelName;
	std::string textureFile1, textureFile2, textureFile3, textureFile4, textureFile5, textureFile6;
	unsigned int textureNum;
	float mix{ 1.0f };
	Vec2 tiling{ 1.0f, 1.0f };
};

