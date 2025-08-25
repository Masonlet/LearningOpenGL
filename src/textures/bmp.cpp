#include <glad/glad.h>
#include "textures/bmp.hpp"
#include "textures/bmpParser.hpp"
#include "utils/log.hpp"

bool BMPTexture::createBMPTexture(std::string path, bool generateMIPMap) {
	BMP temp;
  if (!parseBMP((std::string(ASSET_DIR) + "/textures/" + path).c_str(), temp)) 
    return error("BMPTexture", "createBMPTexture", "Failed to parse " + path);

  unsigned int tex = 0;
	glGenTextures(1, &tex);
	glBindTexture(GL_TEXTURE_2D, tex);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, generateMIPMap ? GL_LINEAR_MIPMAP_LINEAR : GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glPixelStorei(GL_UNPACK_ALIGNMENT, 1); 

  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB8, temp.infoHeader.width, temp.infoHeader.height, 0, GL_RGB, GL_UNSIGNED_BYTE, temp.pixels.data());

  if (generateMIPMap) glGenerateMipmap(GL_TEXTURE_2D);

	index  = tex;
  width  = temp.infoHeader.width;
  height = temp.infoHeader.height;
	return true;
}

bool BMPTexture::createBMPCubeTexture(std::string posFileNameX, std::string negFileNameX,
	                                    std::string posFileNameY, std::string negFileNameY,
	                                    std::string posFileNameZ, std::string negFileNameZ, 
                                      bool isSeamless, bool generateMIPMap) {
  BMP faces[6]; 
  const std::string basePath = std::string(ASSET_DIR) + "/textures/";
  const std::string paths[6] = { basePath + posFileNameX, basePath + negFileNameX, basePath + posFileNameY, basePath + negFileNameY, basePath + posFileNameZ, basePath + negFileNameZ };
  for (int i = 0; i < 6; ++i) {
    if (!parseBMP(paths[i].c_str(), faces[i])) 
      return error("BMPTexture", "createCubeBMPTexture", "Failed to parse " + paths[i]);
    if (i != 0 && (faces[i].infoHeader.width != faces[0].infoHeader.width || faces[i].infoHeader.height != faces[0].infoHeader.height))
      return error("BMPTexture", "createCubeBMPTexture", "Cube faces must have identical dimensions");
  }

  unsigned int tex = 0;
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
  for (int i = 0; i < 6; ++i) glTexImage2D(targets[i], 0, GL_RGB8, faces[i].infoHeader.width, faces[i].infoHeader.height, 0, GL_RGB, GL_UNSIGNED_BYTE, faces[i].pixels.data());   
  if (generateMIPMap) glGenerateMipmap(GL_TEXTURE_CUBE_MAP);

  index = tex;
  width = faces[0].infoHeader.width;
  height = faces[0].infoHeader.height;
  isCube = true;
  return true;
}