#pragma once

#include "scene/scene.hpp"
#include "utils/parserObjects.hpp"

bool parseModel(const unsigned char*& p, ModelData& out);
bool parseLight(const unsigned char*& p, Light& out);
bool parseCamera(const unsigned char*& p, Camera& out);

bool parseTexture(const unsigned char*& p, BMPTexture& out);
bool parseCubeTexture(const unsigned char*& p, BMPTexture& out);
bool parseTextureConnection(const unsigned char*& p, std::string& model, unsigned int& slot, std::string& texture, float& mix);

bool parseTriangle(const unsigned char*& p, Triangle& out);
bool parseGrid(const unsigned char*& p, Grid& out);

bool parseMaze(const unsigned char*& p, ParsedMaze& out);
bool parseMazeData(const unsigned char*& p, std::string& mazeName, std::vector<std::vector<bool>>& layout);