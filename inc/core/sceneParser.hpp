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
	Vec4 position;
	Vec3 rotation;
	Vec3 scale;
	Vec4 colour;
	ColourMode colourMode;
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
struct ParsedLight {
	unsigned int index;
	Vec4 position;
	Vec4 diffuse;
	Vec4 atten;
	Vec4 direction;
	Vec4 param1;
	Vec4 param2;
};
struct ParsedMaze {
	std::string floorType;
	std::string wallType;
	std::vector<std::vector<bool>> layout;
};

const unsigned char* parseModel(const unsigned char* p, ParsedModel& out);
const unsigned char* parseTriangle(const unsigned char* p, ParsedTriangle& out);
const unsigned char* parseCubeGrid(const unsigned char* p, ParsedGrid& out);
const unsigned char* parseSquareGrid(const unsigned char* p, ParsedGrid& out);
const unsigned char* parseLight(const unsigned char* p, ParsedLight& out);

const unsigned char* parseMaze(const unsigned char* p, ParsedMaze& out);
const unsigned char* parseMazeData(const unsigned char* p, ParsedMaze& out);