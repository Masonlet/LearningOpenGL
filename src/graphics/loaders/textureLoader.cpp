#include "graphics/loaders/textureLoader.hpp"
#include "graphics/objects/texture.hpp"
#include "graphics/parsers/bmpParser.hpp"
#include "utils/log.hpp"
#include <glad/glad.h>

bool TextureLoader::loadTexture2D(const std::string & path, Texture & outTexture) {
  if (!outTexture.empty()) return error("TextureLoader", "loadTexture2D", "Attempting to load non-empty texture object: " + path);

  if (!parseBMP(path.c_str(), outTexture))
    return error("TextureLoader", "loadTexture2D", "Failed to parse BMP: " + path);

  return true;
}
bool TextureLoader::loadCubeFaces(const std::string(&facePaths)[6], Texture facesOut[6]) {
  for (int i = 0; i < 6; ++i)
    if (!parseBMP(facePaths[i].c_str(), facesOut[i]))
      return error("TextureLoader", "loadCubeFaces", "Failed face[" + std::to_string(i) + "]: " + facePaths[i]);

  return true;
}

bool TextureLoader::uploadTexture2D(Texture& texture, bool generateMIPMap) {
  if (texture.empty()) return error("TextureLoader", "uploadTexture2D", "Attempting to upload empty texture");

  glGenTextures(1, &texture.id);
  glBindTexture(GL_TEXTURE_2D, texture.id);
  glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

  const GLenum src = (texture.pixelSize == 4) ? GL_RGBA : GL_RGB;
  const GLint  internal = (texture.pixelSize == 4) ? GL_RGBA8 : GL_RGB8;
  glTexImage2D(GL_TEXTURE_2D, 0, internal, texture.width, texture.height, 0, src, GL_UNSIGNED_BYTE, texture.pixels);

  if (generateMIPMap) glGenerateMipmap(GL_TEXTURE_2D);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, generateMIPMap ? GL_LINEAR_MIPMAP_LINEAR : GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
  glBindTexture(GL_TEXTURE_2D, 0);

  GLenum err = glGetError();
  if (err != GL_NO_ERROR) {
    if (texture.id) glDeleteTextures(1, &texture.id);
    return error("TextureLoader", "uploadTexture2D", "OpenGL error " + std::to_string(err));
  }

  texture.freePixels();
  return true;
}
bool TextureLoader::uploadTextureCube(const Texture faces[6], Texture& cubeOut, bool generateMIPMap) {
  const int w = faces[0].width, h = faces[0].height; 
  const uint8_t bpp = faces[0].pixelSize;
  for (int i = 0; i < 6; ++i)
    if (!faces[i].pixels || faces[i].width != w || faces[i].height != h || faces[i].pixelSize != bpp)
      return error("TextureLoader", "uploadTextureCube", "Inconsistent cube faces");

  glGenTextures(1, &cubeOut.id);
  glBindTexture(GL_TEXTURE_CUBE_MAP, cubeOut.id);
  glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

  const GLenum src      = (bpp == 4) ? GL_RGBA : GL_RGB;
  const GLint  internal = (bpp == 4) ? GL_RGBA8 : GL_RGB8;
  const GLenum targets[6] = {
    GL_TEXTURE_CUBE_MAP_POSITIVE_X, GL_TEXTURE_CUBE_MAP_NEGATIVE_X,
    GL_TEXTURE_CUBE_MAP_POSITIVE_Y, GL_TEXTURE_CUBE_MAP_NEGATIVE_Y,
    GL_TEXTURE_CUBE_MAP_POSITIVE_Z, GL_TEXTURE_CUBE_MAP_NEGATIVE_Z
  };

  for (int i = 0; i < 6; ++i)
    glTexImage2D(targets[i], 0, internal, w, h, 0, src, GL_UNSIGNED_BYTE, faces[i].pixels);

  if (generateMIPMap) glGenerateMipmap(GL_TEXTURE_CUBE_MAP);
  glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, generateMIPMap ? GL_LINEAR_MIPMAP_LINEAR : GL_LINEAR);
  glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
  glBindTexture(GL_TEXTURE_CUBE_MAP, 0);

  GLenum err = glGetError();
  if (err != GL_NO_ERROR) {
    if (cubeOut.id) glDeleteTextures(1, &cubeOut.id);
    return error("TextureLoader", "uploadTextureCube", "OpenGL error " + std::to_string(err));
  }

  cubeOut.width = w; cubeOut.height = h; cubeOut.pixelSize = bpp;
  cubeOut.freePixels(); 
  return true;
}