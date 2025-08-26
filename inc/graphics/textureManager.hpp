#pragma once

#include "graphics/texture.hpp"
#include <string>
#include <map>

class TextureManager {
public:
	TextureManager() = default;
	~TextureManager();

	bool uploadTextureToGPU(const std::string& name, Texture& texture, const bool generateMIPMap);
	bool uploadCubeTextureToGPU(const std::string& name, const Texture faces[6], bool generateMIPMap);

	bool findTexture(const std::string& name) const;
	bool getTexture(const std::string& name, Texture*& dataOut);

	unsigned int getTextureID(const std::string& name) const;

private:
	std::map<std::string, Texture> nameToTextures;
};