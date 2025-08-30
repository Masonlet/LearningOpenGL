#pragma once

struct Texture;
#include <string>
#include <array>

struct TextureLoader {
	bool loadTexture2D(const std::string& path, Texture& outTexture);
	bool loadCubeFaces(const std::string(&facePaths)[6], Texture facesOut[6]);

	bool uploadTexture2D(Texture& texture, bool generateMIPMap);
	bool uploadTextureCube(const Texture faces[6], Texture& cubeOut, bool generateMIPMap);
};