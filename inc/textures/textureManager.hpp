#pragma once

#include "textures/bmp.hpp"
#include <string>
#include <map>

class TextureManager {
public:
	~TextureManager();

	unsigned int getTextureIDFromName(const std::string& textureFileName);

	void SetPath(const char* pathIn) { path = pathIn; }

	bool Create2DBMPTexture(const char* textureFileName, bool generateMIPMap);
	bool CreateCubeBMPTexture(std::string cubeMapName,
		std::string posXfileName, std::string negXfileName,
		std::string posYfileName, std::string negYfileName,
		std::string posZfileName, std::string negZfileName,
		bool bIsSeamless, bool generateMIPMap);

private:
	std::string path;
	std::map<std::string, BMPTexture*> nameToTexture;
};