#pragma once

#include "objects/texture.hpp"
#include "loaders/textureLoader.hpp"
#include <string>
#include <map>

class TextureManager {
public:
	TextureManager() = default;
	~TextureManager();

	bool addTexture(const std::string& name, const std::string& filePath);
	bool addCubeTexture(const std::string& name, const std::string(&facePaths)[6]);

	bool findTexture(const std::string& name) const;
	bool getTexture(const std::string& name, Texture*& dataOut);
	unsigned int getTextureID(const std::string& name) const;

private:
	TextureLoader loader;
	std::map<std::string, Texture> nameToTextures;
};