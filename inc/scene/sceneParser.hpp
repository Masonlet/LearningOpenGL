#pragma once

#include "math/vec3.hpp"
#include "math/vec4.hpp"
#include "core/colour.hpp"
#include "math/geometry.hpp"

#include "scene/scene.hpp"

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
	ColourMode colourMode;
};

struct ParsedMaze {
	std::string mazeName;
	std::string floorType;
	std::string wallType;
	std::string layoutName;
	std::vector<std::vector<bool>> layout;
};

const unsigned char* parseModel(const unsigned char* p, ParsedModel& out);
const unsigned char* parseLight(const unsigned char* p, ParsedLight& out);
const unsigned char* parseCamera(const unsigned char* p, ParsedCamera& out);

const unsigned char* parseTriangle(const unsigned char* p, ParsedTriangle& out);
const unsigned char* parseGrid(const unsigned char* p, ParsedGrid& out);

const unsigned char* parseMaze(const unsigned char* p, ParsedMaze& out);
bool parseMazeData(const unsigned char* p, ParsedMaze& maze);