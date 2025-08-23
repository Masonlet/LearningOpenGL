#pragma once

#include "math/vec3.hpp"
#include "math/vec4.hpp"
#include "core/colour.hpp"
#include "math/geometry.hpp"

#include <string>
#include <vector>


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
	std::string name;
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
  std::string textureFile;
	unsigned int index;
	float mix{ 1.0f };
	Vec2 tiling{ 1.0f, 1.0f };
};
struct ParsedTextureCube {
	std::string textureFile;
	std::string textureFile1, textureFile2, textureFile3, textureFile4, textureFile5, textureFile6;
	unsigned int index;
	float mix{ 1.0f };
	Vec2 tiling{ 1.0f, 1.0f };
};

