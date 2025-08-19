#include "textures/textureManager.hpp"

TextureManager::~TextureManager() {
	std::map<std::string, BMPTexture*>::iterator it = this->nameToTexture.begin();
	for (it; it != this->nameToTexture.end(); ++it) {
		delete it->second;     
		it->second = nullptr;
	}
	this->nameToTexture.clear();
}

unsigned int TextureManager::getTextureIDFromName(const std::string& textureFileName) {
	std::map<std::string, BMPTexture*>::iterator it = this->nameToTexture.find(textureFileName);
	return (it == this->nameToTexture.end()) ? 0 
		                                       : static_cast<unsigned>(it->second->getTextureNum());
}

bool TextureManager::Create2DBMPTexture(const char* textureFileName, bool generateMIPMap) {
	std::string pathName = std::string(ASSET_DIR) + "/textures/" + textureFileName;

	BMPTexture* tempTexture = new BMPTexture();
	if (!tempTexture->CreateBMPTexture(textureFileName, pathName, generateMIPMap)) {
		delete tempTexture;
		return false;
	}

	this->nameToTexture[textureFileName] = tempTexture;
	return true;
}
bool TextureManager::CreateCubeBMPTexture(std::string cubeMapName,
	std::string posXfileName, std::string negXfileName,
	std::string posYfileName, std::string negYfileName,
	std::string posZfileName, std::string negZfileName,
	bool bIsSeamless, bool generateMIPMap) {
	const std::string dir = std::string(ASSET_DIR) + "/textures/";
	std::string posXfileNamePath = dir + posXfileName;
	std::string negXfileNamePath = dir + negXfileName;
	std::string posYfileNamePath = dir + posYfileName;
	std::string negYfileNamePath = dir + negYfileName;
	std::string posZfileNamePath = dir + posZfileName;
	std::string negZfileNamePath = dir + negZfileName;

	BMPTexture* tempTexture = new BMPTexture();
	if (!tempTexture->CreateCubeBMPTexture(cubeMapName,
		posXfileNamePath, negXfileNamePath,
		posYfileNamePath, negYfileNamePath,
		posZfileNamePath, negZfileNamePath,
		bIsSeamless, generateMIPMap)
		) {
		delete tempTexture;
		return false;
	}

	this->nameToTexture[cubeMapName] = tempTexture;
	return true;
}