#pragma once

#include "textures/bmp.hpp"
#include <string>
#include <map>

class TextureManager {
public:
	~TextureManager();

	unsigned int getTextureIDFromName(const std::string& textureFileName);

	bool Create2DBMPTexture(const char* textureFileName, bool generateMIPMap);
	bool createCubeBMPTexture(std::string cubeMapName,
		std::string posXfileName, std::string negXfileName,
		std::string posYfileName, std::string negYfileName,
		std::string posZfileName, std::string negZfileName,
		bool bIsSeamless, bool generateMIPMap);

private:
	std::map<std::string, BMPTexture*> nameToTexture;
};