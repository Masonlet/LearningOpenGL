#pragma once

#include "scene/scene.hpp"
#include "utils/parserObjects.hpp"

bool parseModel(const unsigned char*& p, ParsedModel& out);
bool parseLight(const unsigned char*& p, ParsedLight& out);
bool parseCamera(const unsigned char*& p, ParsedCamera& out);

bool parseTexture(const unsigned char*& p, ParsedTexture& out);
bool parseTextureCube(const unsigned char*& p, ParsedTextureCube& out);

bool parseTriangle(const unsigned char*& p, ParsedTriangle& out);
bool parseGrid(const unsigned char*& p, ParsedGrid& out);

bool parseMaze(const unsigned char*& p, ParsedMaze& out);
bool parseMazeData(const unsigned char*& p, ParsedMaze& maze);