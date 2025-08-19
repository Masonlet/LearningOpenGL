#pragma once

#include "scene/scene.hpp"
#include "utils/parserObjects.hpp"

const unsigned char* parseModel(const unsigned char* p, ParsedModel& out);
const unsigned char* parseLight(const unsigned char* p, ParsedLight& out);
const unsigned char* parseCamera(const unsigned char* p, ParsedCamera& out);

const unsigned char* parseTexture(const unsigned char* p, ParsedTexture& out);
const unsigned char* parseTextureCube(const unsigned char* p, ParsedTextureCube& out);

const unsigned char* parseTriangle(const unsigned char* p, ParsedTriangle& out);
const unsigned char* parseGrid(const unsigned char* p, ParsedGrid& out);

const unsigned char* parseMaze(const unsigned char* p, ParsedMaze& out);
bool parseMazeData(const unsigned char* p, ParsedMaze& maze);