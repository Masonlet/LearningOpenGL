#include "scene/sceneParser.hpp"
#include "utils/parser.hpp"

bool parseModel(const unsigned char*& p, ParsedModel& out) {
  PARSE_OR_FALSE(parseBool, out.isVisible, "model enabled");
  PARSE_OR_FALSE(parseBool, out.isLighted, "model lighting");
  PARSE_STRING_OR_FALSE(p, out.name, 64, "model name");
  PARSE_STRING_OR_FALSE(p, out.path, 128, "model path");
  PARSE_OR_FALSE(parseVec3, out.position, "model position");
  PARSE_OR_FALSE(parseVec3, out.rotation, "model rotation");
  PARSE_OR_FALSE(parseVec3, out.scale, "model scale");
  if (!parseColour(p, out.colour, out.colourMode)) return false;
  PARSE_OR_FALSE(parseVec4, out.specular, "model specular");
  return true;
}
bool parseLight(const unsigned char*& p, ParsedLight& out) {
  PARSE_OR_FALSE(parseBool, out.isEnabled, "light enabled");
  PARSE_STRING_OR_FALSE(p, out.name, 64, "light name");
  PARSE_OR_FALSE(parseLightType, out.param1Type, "light type");
  PARSE_OR_FALSE(parseVec3, out.position, "light position");
  PARSE_OR_FALSE(parseVec4, out.diffuse, "light diffuse");
  PARSE_OR_FALSE(parseVec4, out.atten, "light attenuation");
  PARSE_OR_FALSE(parseVec4, out.direction, "light direction");
  PARSE_OR_FALSE(parseVec3, out.param1Direction, "light param1");
  return true;
}
bool parseCamera(const unsigned char*& p, ParsedCamera& out) {
  PARSE_OR_FALSE(parseBool, out.isEnabled, "camera enabled");
  PARSE_STRING_OR_FALSE(p, out.name, 64, "camera name");
  PARSE_OR_FALSE(parseCameraType, out.type, "camera type");
  PARSE_OR_FALSE(parseVec3, out.position, "camera position");
  PARSE_OR_FALSE(parseFloat, out.yaw, "camera yaw");
  PARSE_OR_FALSE(parseFloat, out.pitch, "camera pitch");
  PARSE_OR_FALSE(parseFloat, out.fov, "camera fov");
  PARSE_OR_FALSE(parseFloat, out.nearPlane, "camera near plane");
  PARSE_OR_FALSE(parseFloat, out.farPlane, "camera far plane");
  PARSE_OR_FALSE(parseFloat, out.speed, "camera speed");
  if (out.type != 0) PARSE_OR_FALSE(parseFloat, out.moveDistance, "camera move distance");
  return true;
}
bool parseTexture(const unsigned char*& p, ParsedTexture& out) {
  PARSE_STRING_OR_FALSE(p, out.modelName, 64, "texture model name");
  PARSE_STRING_OR_FALSE(p, out.textureFile, 128, "texture file");
  PARSE_OR_FALSE(parseUInt, out.textureNum, "texture num");
  PARSE_OR_FALSE(parseFloat, out.mix, "texture mix");
  PARSE_OR_FALSE(parseVec2, out.tiling, "texture tiling");
  return true;
}
bool parseTextureCube(const unsigned char*& p, ParsedTextureCube& out) {
  PARSE_STRING_OR_FALSE(p, out.modelName, 64, "texture model name");
  PARSE_STRING_OR_FALSE(p, out.textureFile1, 128, "texture file");
  PARSE_STRING_OR_FALSE(p, out.textureFile2, 128, "texture file");
  PARSE_STRING_OR_FALSE(p, out.textureFile3, 128, "texture file");
  PARSE_STRING_OR_FALSE(p, out.textureFile4, 128, "texture file");
  PARSE_STRING_OR_FALSE(p, out.textureFile5, 128, "texture file");
  PARSE_STRING_OR_FALSE(p, out.textureFile6, 128, "texture file");
  PARSE_OR_FALSE(parseUInt, out.textureNum, "texture num");
  PARSE_OR_FALSE(parseFloat, out.mix, "texture mix");
  PARSE_OR_FALSE(parseVec2, out.tiling, "texture tiling");
  return true;
}

bool parseTriangle(const unsigned char*& p, ParsedTriangle& out) {
  PARSE_STRING_OR_FALSE(p, out.name, 64, "triangle name");

  Vec3 temp;
  PARSE_OR_FALSE(parseVec3, temp, "triangle position");
  out.transform.position = { temp, 0.0f };

  PARSE_OR_FALSE(parseVec3, out.transform.rotation, "triangle rotation");
  PARSE_OR_FALSE(parseVec3, out.transform.scale, "triangle size");
  if (!parseColour(p, out.colour, out.colourMode)) return false;
  return true;
}
bool parseGrid(const unsigned char*& p, ParsedGrid& out) {
  PARSE_OR_FALSE(parseUInt, out.layout.count, "cubeGrid count");
  PARSE_OR_FALSE(parseFloat, out.layout.spacing, "cubeGrid spacing");
  PARSE_OR_FALSE(parseVec3, out.layout.start, "cubeGrid start position");
  PARSE_OR_FALSE(parseVec3, out.layout.rotation, "cubeGrid rotation");
  PARSE_OR_FALSE(parseVec3, out.layout.scale, "cubeGrid scale");
  if (!parseColour(p, out.colour, out.colourMode)) return false;
  return true;
} 

bool parseMaze(const unsigned char*& p, ParsedMaze& out) {
  out.layout.clear();
  PARSE_STRING_OR_FALSE(p, out.mazeName, 64, "maze name");
  PARSE_OR_FALSE(parseFloat, out.spacing, "maze spacing");
  PARSE_OR_FALSE(parseVec3, out.pos, "maze position");
  PARSE_OR_FALSE(parseVec3, out.rot, "maze rotation");
  PARSE_OR_FALSE(parseUInt, out.wallHeight, "maze wall height");
  PARSE_OR_FALSE(parseBool, out.hasRoof, "maze ceiling flag");
  PARSE_STRING_OR_FALSE(p, out.floorType1, 64, "floor mesh 1");
  PARSE_STRING_OR_FALSE(p, out.floorType2, 64, "floor mesh 2");
  PARSE_STRING_OR_FALSE(p, out.floorType3, 64, "floor mesh 3");
  PARSE_STRING_OR_FALSE(p, out.floorType4, 64, "floor mesh 4");
  PARSE_STRING_OR_FALSE(p, out.floorType5, 64, "floor mesh 5");
  PARSE_STRING_OR_FALSE(p, out.floorType6, 64, "floor mesh 6");
  PARSE_STRING_OR_FALSE(p, out.floorWallType, 64, "floor wall mesh");
  PARSE_STRING_OR_FALSE(p, out.wallType1, 64, "wall mesh 1");
  PARSE_STRING_OR_FALSE(p, out.wallType2, 64, "wall mesh 2");
  PARSE_STRING_OR_FALSE(p, out.wallType3, 64, "wall mesh 3");
  PARSE_STRING_OR_FALSE(p, out.wallType4, 64, "wall mesh 4");
  PARSE_STRING_OR_FALSE(p, out.wallType5, 64, "wall mesh 5");
  PARSE_STRING_OR_FALSE(p, out.wallType6, 64, "wall mesh 6");
  PARSE_STRING_OR_FALSE(p, out.entranceType, 64, "entrance mesh");
  PARSE_STRING_OR_FALSE(p, out.exitType, 64, "exit mesh");
  PARSE_STRING_OR_FALSE(p, out.exteriorWallType, 64, "exterior wall mesh");
  PARSE_OR_FALSE(parseVec3, out.baseRot, "maze base wall rotation");
  PARSE_STRING_OR_FALSE(p, out.layoutName, 64, "layout name");
  return true;
}
static bool parseMazeLayoutRow(const char* p, const char* lineEnd, std::vector<bool>& outRow) {
  outRow.clear();
  for (const char* s = p; s < lineEnd; ++s) {
    const char c = *s;
    if      (c == '.')      outRow.push_back(true);
    else if (c == 'X')      outRow.push_back(false);
    else {
      fprintf(stderr, "[SceneLoader ERROR] Invalid character '%c' in maze layout line: \"%.*s\"\n", c, static_cast<int>(lineEnd - p), p);
      return false;
    }
  }
  return !outRow.empty();
}
bool parseMazeData(const unsigned char*& p, ParsedMaze& maze) {
  skipWhitespace(p);
  PARSE_STRING_OR_FALSE(p, maze.layoutName, 64, "layout name");
  maze.layout.clear();

  while (*p) {
    const unsigned char* nextLine = p;
    skipToNextLine(nextLine);
    const unsigned char* endLine = nextLine;
    trimEOL(p, endLine);

    if (endLine == p) {
      p = nextLine;
      continue;
    }

    unsigned char token[64]{};
		const unsigned char* peek = p;
    if (parseToken(peek, token, sizeof(token))) {
      if (strcmp(reinterpret_cast<const char*>(token), maze.layoutName.c_str()) == 0) {
        p = nextLine;
        break;
      };
    }

    std::vector<bool> row;
    if (parseMazeLayoutRow(reinterpret_cast<const char*>(p), reinterpret_cast<const char*>(endLine), row))
      maze.layout.push_back(std::move(row));

    p = nextLine;
  }

  if (maze.layout.empty()) {
    fprintf(stderr, "[SceneLoader ERROR] Maze layout was empty or not found: %s\n", maze.layoutName.c_str());
    return false;
  }

  return true;
}