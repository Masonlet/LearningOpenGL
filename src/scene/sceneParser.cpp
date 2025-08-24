#include "scene/sceneParser.hpp"
#include "utils/parser.hpp"
#include "utils/log.hpp"

bool parseModel(const unsigned char*& p, ModelData& out) {
  PARSE_OR(return false, parseBool, out.isVisible, "model enabled");
  PARSE_OR(return false, parseBool, out.isLighted, "model lighting");
  PARSE_STRING_OR(return false, p, out.name, 64, "model name");
  PARSE_STRING_OR(return false, p, out.meshPath, 128, "model path");
  PARSE_OR(return false, parseVec3, out.pos, "model position");
  PARSE_OR(return false, parseVec3, out.rot, "model rotation");
  PARSE_OR(return false, parseVec3, out.size, "model scale");
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
bool parseTriangle(const unsigned char*& p, Triangle& out) {
  PARSE_STRING_OR(return false, p, out.name, 64, "triangle name");
  PARSE_OR(return false, parseVec3, out.pos, "triangle position");
  PARSE_OR(return false, parseVec3, out.rot, "triangle rotation");
  PARSE_OR(return false, parseVec3, out.size, "triangle size");
  return parseColour(p, out.colour, out.colourMode);
}
bool parseGrid(const unsigned char*& p, Grid& out) {
  PARSE_OR(return false, parseUInt, out.count, "cubeGrid count");
  PARSE_OR(return false, parseFloat, out.spacing, "cubeGrid spacing");
  PARSE_OR(return false, parseVec3, out.start, "cubeGrid start position");
  PARSE_OR(return false, parseVec3, out.rot, "cubeGrid rotation");
  PARSE_OR(return false, parseVec3, out.size, "cubeGrid scale");
  return parseColour(p, out.colour, out.colourMode);
} 

bool parseMaze(const unsigned char*& p, ParsedMaze& out) {
  out.layout.clear();
  PARSE_STRING_OR(return false, p, out.name, 64, "maze name");
  PARSE_OR(return false, parseFloat, out.spacing, "maze spacing");
  PARSE_OR(return false, parseVec3, out.pos, "maze position");
  PARSE_OR(return false, parseVec3, out.rot, "maze rotation");
  PARSE_OR(return false, parseUInt, out.wallHeight, "maze wall height");
  PARSE_OR(return false, parseBool, out.hasRoof, "maze ceiling flag");
  PARSE_STRING_OR(return false, p, out.floorType1, 64, "floor mesh 1");
  PARSE_STRING_OR(return false, p, out.floorType2, 64, "floor mesh 2");
  PARSE_STRING_OR(return false, p, out.floorType3, 64, "floor mesh 3");
  PARSE_STRING_OR(return false, p, out.floorType4, 64, "floor mesh 4");
  PARSE_STRING_OR(return false, p, out.floorType5, 64, "floor mesh 5");
  PARSE_STRING_OR(return false, p, out.floorType6, 64, "floor mesh 6");
  PARSE_STRING_OR(return false, p, out.floorWallType, 64, "floor wall mesh");
  PARSE_STRING_OR(return false, p, out.wallType1, 64, "wall mesh 1");
  PARSE_STRING_OR(return false, p, out.wallType2, 64, "wall mesh 2");
  PARSE_STRING_OR(return false, p, out.wallType3, 64, "wall mesh 3");
  PARSE_STRING_OR(return false, p, out.wallType4, 64, "wall mesh 4");
  PARSE_STRING_OR(return false, p, out.wallType5, 64, "wall mesh 5");
  PARSE_STRING_OR(return false, p, out.wallType6, 64, "wall mesh 6");
  PARSE_STRING_OR(return false, p, out.entranceType, 64, "entrance mesh");
  PARSE_STRING_OR(return false, p, out.exitType, 64, "exit mesh");
  PARSE_STRING_OR(return false, p, out.exteriorWallType, 64, "exterior wall mesh");
  PARSE_OR(return false, parseVec3, out.baseRot, "maze base wall rotation");
  return true;
}
static bool parseMazeLayoutRow(const unsigned char* p, const unsigned char* lineEnd, std::vector<bool>& outRow) {
  outRow.clear();
  for (const char* s = reinterpret_cast<const char*>(p); s < reinterpret_cast<const char*>(lineEnd); ++s) {
    const char c = *s;
    if (c == '.')      outRow.push_back(true);
    else if (c == 'X') outRow.push_back(false);
    else return error("SceneParser", "parseMazeLayoutRow", "Invalid character" + std::string(1, c));
  }
  return outRow.empty() ? error("SceneParser", "parseMazeLayoutRow", "Empty row found") : true;
}
bool parseMazeData(const unsigned char*& p, std::string& mazeName, std::vector<std::vector<bool>>& layout) {
  p = skipWhitespace(p);
  PARSE_STRING_OR(return false, p, mazeName, 64, "Maze name");
  layout.clear();

  while (*p) {
    const unsigned char* nextLine = skipToNextLine(p);
    const unsigned char* endLine = trimEOL(p, nextLine);

    if (endLine <= p) {
      p = nextLine;
      continue;
    }

    unsigned char token[64]{};
    const unsigned char* peek = p;
    if (parseToken(peek, token, sizeof(token)))
      if (strcmp(reinterpret_cast<const char*>(token), mazeName.c_str()) == 0) {
        p = nextLine;
        break;
      }
        
    std::vector<bool> row;
    if (!parseMazeLayoutRow(p, endLine, row)) return false;
    layout.push_back(row);
    p = nextLine;
  }

  return layout.empty() ? error("SceneParser", "parseMazeData", "No maze layout data found") : true;
}