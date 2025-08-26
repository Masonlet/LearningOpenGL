#include "graphics/textureManager.hpp"
#include "utils/log.hpp"
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

const int targets[6] = {
    GL_TEXTURE_CUBE_MAP_POSITIVE_X, GL_TEXTURE_CUBE_MAP_NEGATIVE_X,
    GL_TEXTURE_CUBE_MAP_POSITIVE_Y, GL_TEXTURE_CUBE_MAP_NEGATIVE_Y,
    GL_TEXTURE_CUBE_MAP_POSITIVE_Z, GL_TEXTURE_CUBE_MAP_NEGATIVE_Z
};
bool TextureManager::uploadTextureToGPU(const std::string& name, Texture& texture, const bool generateMIPMap) {
  if (texture.empty())   return error("TextureManager", "uploadTextureToGPU", "Invalid texture data");
  if (findTexture(name)) return error("TextureManager", "uploadTextureToGPU", "Texture data exists already");

  glGenTextures(1, &texture.id);
  glBindTexture(GL_TEXTURE_2D, texture.id);

  glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

  const int srcFormat = (texture.pixelSize == 4) ? GL_RGBA : GL_RGB;
  const int internal  = (texture.pixelSize == 4) ? GL_RGBA8 : GL_RGB8;
  glTexImage2D(GL_TEXTURE_2D, 0, internal, texture.width, texture.height, 0, srcFormat, GL_UNSIGNED_BYTE, texture.pixels);

  if (generateMIPMap) glGenerateMipmap(GL_TEXTURE_2D);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, generateMIPMap ? GL_LINEAR_MIPMAP_LINEAR : GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

  glBindTexture(GL_TEXTURE_2D, 0);

  GLenum err = glGetError();
  if (err != GL_NO_ERROR) {
    if (texture.id) glDeleteTextures(1, &texture.id);
    return error("TextureManager", "uploadTextureToGPU", "OpenGL error " + std::to_string(err) + "after uploading Texture " + name);
  }

  texture.freePixels();
  nameToTextures[name] = std::move(texture);
  return true;
}
bool TextureManager::uploadCubeTextureToGPU(const std::string& name, const Texture faces[6], bool generateMIPMap) {
	if (findTexture(name)) return error("TextureManager", "uploadCubeTextureToGPU", "Texture data exists already");
  
  int32_t w = faces[0].width, h = faces[0].height;
  uint8_t bpp = faces[0].pixelSize;
  for (int i = 0; i < 6; ++i) 
    if (!faces[i].pixels || faces[i].width != w || faces[i].height != h || faces[i].pixelSize != bpp)
      return error("TextureManager", "uploadCubeTextureToGPU", "Inconsistent cubemap faces for " + name);

  Texture cube;
  glGenTextures(1, &cube.id);
  glBindTexture(GL_TEXTURE_CUBE_MAP, cube.id);

  glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
  const GLenum srcFormat = (bpp == 4) ? GL_RGBA : GL_RGB;
  const GLint  internal  = (bpp == 4) ? GL_RGBA8 : GL_RGB8;

  for (int i = 0; i < 6; ++i) glTexImage2D(targets[i], 0, internal, w, h, 0, srcFormat, GL_UNSIGNED_BYTE, faces[i].pixels);
 
  if (generateMIPMap) glGenerateMipmap(GL_TEXTURE_CUBE_MAP);
  glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, generateMIPMap ? GL_LINEAR_MIPMAP_LINEAR : GL_LINEAR);
  glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

  glBindTexture(GL_TEXTURE_CUBE_MAP, 0);

  GLenum err = glGetError();
  if (err != GL_NO_ERROR) {
    if (cube.id) glDeleteTextures(1, &cube.id);
    return error("TextureManager", "uploadCubeTextureToGPU", "OpenGL error " + std::to_string(err) + " after uploading cubemap: " + name);
  }

  cube.width = w;
  cube.height = h;
  cube.pixelSize = bpp;
  cube.freePixels();
  nameToTextures[name] = std::move(cube);
  return true;
}