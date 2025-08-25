#include "scene/sceneParser.hpp"
#include "utils/parser.hpp"
#include "utils/log.hpp"

bool parseModel(const unsigned char*& p, Model& out) {
  PARSE_OR(return false, parseBool, out.isVisible, "model enabled");
  PARSE_OR(return false, parseBool, out.isLighted, "model lighting");
  PARSE_STRING_OR(return false, p, out.name, 64, "model name");
  PARSE_STRING_OR(return false, p, out.meshPath, 128, "model path");
  Vec3 temp;
  PARSE_OR(return false, parseVec3, temp, "model position");
  out.transform.pos = { temp, 1.0f };
  PARSE_OR(return false, parseVec3, out.transform.rot, "model rotation");
  PARSE_OR(return false, parseVec3, out.transform.size, "model scale");
  if (!parseColour(p, out.colour, out.colourMode)) return false;
  PARSE_OR(return false, parseVec4, out.specular, "model specular");
  return true;
}
bool parseLight(const unsigned char*& p, Light& out) {
  PARSE_OR(return false, parseBool, out.enabled, "light enabled");
  PARSE_STRING_OR(return false, p, out.name, 64, "light name");
  PARSE_OR(return false, parseLightType, out.type, "light type");
  PARSE_OR(return false, parseVec3, out.pos, "light position");
  PARSE_OR(return false, parseVec4, out.diffuse, "light diffuse");
  PARSE_OR(return false, parseVec4, out.attenuation, "light attenuation");
  PARSE_OR(return false, parseVec3, out.direction, "light direction");
  PARSE_OR(return false, parseVec2, out.param1, "light param1");
  return true;
}
bool parseCamera(const unsigned char*& p, Camera& out) {
  PARSE_OR(return false, parseBool, out.enabled, "camera enabled");
  PARSE_STRING_OR(return false, p, out.name, 64, "camera name");
  PARSE_OR(return false, parseCameraType, out.type, "camera type");
  PARSE_OR(return false, parseVec3, out.pos, "camera position");
  PARSE_OR(return false, parseFloat, out.yaw, "camera yaw");
  PARSE_OR(return false, parseFloat, out.pitch, "camera pitch");
  PARSE_OR(return false, parseFloat, out.fov, "camera fov");
  PARSE_OR(return false, parseFloat, out.nearPlane, "camera near plane");
  PARSE_OR(return false, parseFloat, out.farPlane, "camera far plane");
  PARSE_OR(return false, parseFloat, out.moveSpeed, "camera speed");
  if (out.type != 0) PARSE_OR(return false, parseFloat, out.moveDistance, "camera move distance");
  return true;
}
bool parseTexture(const unsigned char*& p, BMPTexture& out) {
  PARSE_STRING_OR(return false, p, out.name, 128, "texture name");
  std::string path;
  PARSE_STRING_OR(return false, p, path, 128, "texture file");
  PARSE_OR(return false, parseFloat, out.mix, "texture mix");
  PARSE_OR(return false, parseVec2, out.tiling, "texture tiling");
  return out.createBMPTexture(path, true);
}
bool parseCubeTexture(const unsigned char*& p, BMPTexture& out) {
  PARSE_STRING_OR(return false, p, out.name, 128, "texture name");
  std::string path1, path2, path3, path4, path5, path6;
  PARSE_STRING_OR(return false, p, path1, 128, "texture file");
  PARSE_STRING_OR(return false, p, path2, 128, "texture file");
  PARSE_STRING_OR(return false, p, path3, 128, "texture file");
  PARSE_STRING_OR(return false, p, path4, 128, "texture file");
  PARSE_STRING_OR(return false, p, path5, 128, "texture file");
  PARSE_STRING_OR(return false, p, path6, 128, "texture file");
  PARSE_OR(return false, parseFloat, out.mix, "texture mix");
  PARSE_OR(return false, parseVec2, out.tiling, "texture tiling");
	return out.createBMPCubeTexture(path1, path2, path3, path4, path5, path6, true, true);
}
bool parseTextureConnection(const unsigned char*& p, std::string& model, unsigned int& slot, std::string& texture, float& mix) {
  PARSE_STRING_OR(return false, p, model, 64, "texture connection model name");
  PARSE_OR(return false, parseUInt, slot, "texture connection slot");
  PARSE_STRING_OR(return false, p, texture, 128, "texture connection name");
  PARSE_OR(return false, parseFloat, mix, "texture connection mix");
  return true;
}
bool parseTriangle(const unsigned char*& p, Model& out) {
  PARSE_STRING_OR(return false, p, out.name, 64, "triangle name");
  Vec3 temp;
  PARSE_OR(return false, parseVec3, temp, "triangle position");
	out.transform.pos = { temp, 1.0f };
  PARSE_OR(return false, parseVec3, out.transform.rot, "triangle rotation");
  PARSE_OR(return false, parseVec3, out.transform.size, "triangle size");
  return parseColour(p, out.colour, out.colourMode);
}
bool parseGrid(const unsigned char*& p, Grid& out) {
  PARSE_OR(return false, parseUInt, out.count, "cubeGrid count");
  PARSE_OR(return false, parseFloat, out.spacing, "cubeGrid spacing");
  Vec3 temp;
  PARSE_OR(return false, parseVec3, temp, "cubeGrid start position");
  out.transform.pos = { temp, 1.0f };
  PARSE_OR(return false, parseVec3, out.transform.rot, "cubeGrid rotation");
  PARSE_OR(return false, parseVec3, out.transform.size, "cubeGrid scale");
  return parseColour(p, out.colour, out.colourMode);
} 