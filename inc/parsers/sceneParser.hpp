#pragma once

#include "objects/model.hpp"
#include "objects/light.hpp"
#include "objects/camera.hpp"
#include "objects/grid.hpp"
#include "objects/textureData.hpp"

bool parseModel(const unsigned char*& p, Model& out);
bool parseLight(const unsigned char*& p, Light& out);
bool parseCamera(const unsigned char*& p, Camera& out);

bool parseTriangle(const unsigned char*& p, Model& out);
bool parseGrid(const unsigned char*& p, Grid& out);

bool parseTexture(const unsigned char*& p, TextureData& out);
bool parseCubeTexture(const unsigned char*& p, TextureData& out);
bool parseTextureConnection(const unsigned char*& p, std::string& model, unsigned int& slot, std::string& texture, float& mix);

