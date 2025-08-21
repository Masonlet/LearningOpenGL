#include <glad/glad.h>
#include "textures/bmp.hpp"
#include "textures/bmpParser.hpp"
#include "utils/log.hpp"

bool BMPTexture::createBMPTexture(std::string name, std::string path, bool generateMIPMap) {
	BMP temp;
	if (!parseBMP(path.c_str(), temp)) return error("BMPTexture", "createBMPTexture", "Failed to parse " + path);

	GLuint tex = 0;
	glGenTextures(1, &tex);
	glBindTexture(GL_TEXTURE_2D, tex);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, generateMIPMap ? GL_LINEAR_MIPMAP_LINEAR : GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

	glPixelStorei(GL_UNPACK_ALIGNMENT, 1); 

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB8, temp.infoHeader.width, temp.infoHeader.height, 0, GL_RGB, GL_UNSIGNED_BYTE, temp.data);

	if (generateMIPMap) glGenerateMipmap(GL_TEXTURE_2D);

	this->textureNum  = (int)tex;
	this->textureName = name;
	this->texturePath = path;
	this->bmp         = std::move(temp); 
	this->isCubeMap   = false;
	this->is2DTexture = true;
	return true;
}

bool BMPTexture::createCubeBMPTexture(std::string cubeMapName,
	std::string posXfileName, std::string negXfileName,
	std::string posYfileName, std::string negYfileName,
	std::string posZfileName, std::string negZfileName,
	bool isSeamless, bool generateMIPMap) {
  BMP faces[6];
  const std::string paths[6] = { posXfileName, negXfileName, posYfileName, negYfileName, posZfileName, negZfileName };
  for (int i = 0; i < 6; ++i) {
    if (!parseBMP(paths[i].c_str(), faces[i])) {
      for (int j = 0; j < i; ++j) delete[] faces[j].data;
			return error("BMPTexture", "createCubeBMPTexture", "Failed to parse " + paths[i]);
    }
    if (faces[i].infoHeader.width != faces[0].infoHeader.width || faces[i].infoHeader.height != faces[0].infoHeader.height) {
      for (int j = 0; j <= i; ++j) delete[] faces[j].data;
			return error("BMPTexture", "createCubeBMPTexture", "Cube faces must have identical dimensions");
    }
  }

  GLuint tex = 0;
  glGenTextures(1, &tex);
  glBindTexture(GL_TEXTURE_CUBE_MAP, tex);

  glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, generateMIPMap ? GL_LINEAR_MIPMAP_LINEAR : GL_LINEAR);
  glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
#ifdef GL_TEXTURE_CUBE_MAP_SEAMLESS
  if (isSeamless) glEnable(GL_TEXTURE_CUBE_MAP_SEAMLESS);
#endif

  glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

  const GLenum targets[6] = {
      GL_TEXTURE_CUBE_MAP_POSITIVE_X, GL_TEXTURE_CUBE_MAP_NEGATIVE_X,
      GL_TEXTURE_CUBE_MAP_POSITIVE_Y, GL_TEXTURE_CUBE_MAP_NEGATIVE_Y,
      GL_TEXTURE_CUBE_MAP_POSITIVE_Z, GL_TEXTURE_CUBE_MAP_NEGATIVE_Z
  };

  for (int i = 0; i < 6; ++i) 
    glTexImage2D(targets[i], 0, GL_RGB8, faces[i].infoHeader.width, faces[i].infoHeader.height, 0, GL_RGB, GL_UNSIGNED_BYTE, faces[i].data);
 
  if (generateMIPMap) glGenerateMipmap(GL_TEXTURE_CUBE_MAP);

  this->textureNum = (int)tex;
  this->textureName = cubeMapName;
  this->texturePath = ""; 
  this->isCubeMap = true;
  this->is2DTexture = false;
  if (this->bmp.data) { delete[] this->bmp.data; this->bmp.data = nullptr; }
  return true;
}