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
	std::string pathName = path + '/' + textureFileName;

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
	std::string posXfileNamePath = this->path + "/" + posXfileName;
	std::string negXfileNamePath = this->path + "/" + negXfileName;
	std::string posYfileNamePath = this->path + "/" + posYfileName;
	std::string negYfileNamePath = this->path + "/" + negYfileName;
	std::string posZfileNamePath = this->path + "/" + posZfileName;
	std::string negZfileNamePath = this->path + "/" + negZfileName;

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