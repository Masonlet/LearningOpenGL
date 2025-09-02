#include "graphics/textureManager.hpp"
#include "starletparsers/utils/log.hpp"
#include <glad/glad.h>
  
TextureManager::~TextureManager() {
  for (std::map<std::string, Texture>::iterator it = nameToTextures.begin(); it != nameToTextures.end(); ++it) {
    unsigned int& id = it->second.id;
    if (id) { glDeleteTextures(1, &id); id = 0; }
  }
  nameToTextures.clear();
}

bool TextureManager::findTexture(const std::string& path) const {
  return nameToTextures.find(path) != nameToTextures.end();
}
bool TextureManager::getTexture(const std::string& name, Texture*& data) {
  std::map<std::string, Texture>::iterator it = nameToTextures.find(name);
  if (it == nameToTextures.end()) return error("TextureManager", "getTexture", "Texture not found: " + name);
  data = &it->second;
  return true;
}
unsigned int TextureManager::getTextureID(const std::string& name) const {
  std::map<std::string, Texture>::const_iterator it = nameToTextures.find(name);
  return (it == nameToTextures.end()) ? 0u : it->second.id;
}

bool TextureManager::addTexture(const std::string& name, const std::string& filePath) {
  if (findTexture(name)) return true;

  Texture texture;
  if (!loader.loadTexture2D(filePath, texture))
    return error("TextureManager", "addTexture", "Failed load: " + filePath);

  if (!loader.uploadTexture2D(texture, true))
    return error("TextureManager", "addTexture", "Failed upload: " + name);

  nameToTextures[name] = std::move(texture);
  return true;
}

bool TextureManager::addCubeTexture(const std::string& name, const std::string(&facePaths)[6]) {
  if (findTexture(name)) return true;

  Texture faces[6];
  if (!loader.loadCubeFaces(facePaths, faces))
    return error("TextureManager", "addCubeTexture", "Failed to load cube map faces for: " + name);

  Texture cube;
  if (!loader.uploadTextureCube(faces, cube, true))
    return error("TextureManager", "addCubeTexture", "Failed to upload cube map: " + name);

  for (int i = 0; i < 6; ++i) faces[i].freePixels();
  nameToTextures[name] = std::move(cube);
  return true;
}