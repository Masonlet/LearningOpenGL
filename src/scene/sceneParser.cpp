#include "scene/sceneParser.hpp"
#include "utils/parser.hpp"

const unsigned char* parseModel(const unsigned char* p, ParsedModel& out) {
  PARSE_STRING_OR_NULL(p, out.name, 64, "model name");
  PARSE_STRING_OR_NULL(p, out.path, 128, "parse path");
  PARSE_OR_FAIL(parseVec3, out.position, "parse position");
  PARSE_OR_FAIL(parseVec3, out.rotation, "parse rotation");
  PARSE_OR_FAIL(parseVec3, out.scale, "parse scale");
  p = parseColour(p, out.colour, out.colourMode);
  PARSE_OR_FAIL(parseVec4, out.specular, "parse specular");
  return p;
}
const unsigned char* parseLight(const unsigned char* p, ParsedLight& out) {
  PARSE_STRING_OR_NULL(p, out.name, 64, "light name");
  p = parseLightType(p, out.param1Type);
  PARSE_OR_FAIL(parseVec3, out.position, "light position");
  PARSE_OR_FAIL(parseVec4, out.diffuse, "light diffuse");
  PARSE_OR_FAIL(parseVec4, out.atten, "light attenuation");
  PARSE_OR_FAIL(parseVec4, out.direction, "light direction");
  PARSE_OR_FAIL(parseVec3, out.param1Direction, "light param1");
  PARSE_OR_FAIL(parseVec4, out.param2, "light param2");
  return p;
}
const unsigned char* parseCamera(const unsigned char* p, ParsedCamera& out) {
	PARSE_STRING_OR_NULL(p, out.name, 64, "camera name");
  p = parseCameraType(p, out.type);
  PARSE_OR_FAIL(parseVec3, out.position, "camera position");
  PARSE_OR_FAIL(parseFloat, out.yaw, "camera yaw");
  PARSE_OR_FAIL(parseFloat, out.pitch, "camera pitch");
  PARSE_OR_FAIL(parseFloat, out.fov, "camera fov");
  PARSE_OR_FAIL(parseFloat, out.nearPlane, "camera near plane");
  PARSE_OR_FAIL(parseFloat, out.farPlane, "camera far plane");
  PARSE_OR_FAIL(parseFloat, out.speed, "camera speed");
  if (out.type != 0) PARSE_OR_FAIL(parseFloat, out.moveDistance, "camera move distance");
  return p;
}

const unsigned char* parseTriangle(const unsigned char* p, ParsedTriangle& out) {
  PARSE_STRING_OR_NULL(p, out.name, 64, "triangle name");

  Vec3 temp;
  PARSE_OR_FAIL(parseVec3, temp, "triangle position");
  out.transform.position = { temp, 0.0f };

  PARSE_OR_FAIL(parseVec3, out.transform.rotation, "triangle rotation");
  PARSE_OR_FAIL(parseVec3, out.transform.scale, "triangle size");

  p = parseColour(p, out.colour, out.colourMode);

  return p;
}
const unsigned char* parseGrid(const unsigned char* p, ParsedGrid& out) {
  PARSE_OR_FAIL(parseUInt, out.layout.count, "cubeGrid count");
  PARSE_OR_FAIL(parseFloat, out.layout.spacing, "cubeGrid spacing");
  PARSE_OR_FAIL(parseVec3, out.layout.start, "cubeGrid start position");
  PARSE_OR_FAIL(parseVec3, out.layout.rotation, "cubeGrid rotation");
  PARSE_OR_FAIL(parseVec3, out.layout.scale, "cubeGrid scale");
  p = parseColour(p, out.colour, out.colourMode);
  return p;
} 

const unsigned char* parseMaze(const unsigned char* p, ParsedMaze& out) {
  out.layout.clear();
  PARSE_STRING_OR_NULL(p, out.mazeName, 64, "maze name");
  PARSE_OR_FAIL(parseFloat, out.spacing, "maze spacing");
  PARSE_OR_FAIL(parseVec3, out.pos, "maze position");
  PARSE_OR_FAIL(parseVec3, out.rot, "maze rotation");
  PARSE_OR_FAIL(parseUInt, out.wallHeight, "maze wall height");
  PARSE_OR_FAIL(parseBool, out.hasRoof, "maze ceiling flag");
  PARSE_STRING_OR_NULL(p, out.floorType1, 64, "floor mesh 1");
  PARSE_STRING_OR_NULL(p, out.floorType2, 64, "floor mesh 2");
  PARSE_STRING_OR_NULL(p, out.floorType3, 64, "floor mesh 3");
  PARSE_STRING_OR_NULL(p, out.floorType4, 64, "floor mesh 4");
  PARSE_STRING_OR_NULL(p, out.floorType5, 64, "floor mesh 5");
  PARSE_STRING_OR_NULL(p, out.floorType6, 64, "floor mesh 6");
  PARSE_STRING_OR_NULL(p, out.floorWallType, 64, "floor wall mesh");
  PARSE_STRING_OR_NULL(p, out.wallType1, 64, "wall mesh 1");
  PARSE_STRING_OR_NULL(p, out.wallType2, 64, "wall mesh 2");
  PARSE_STRING_OR_NULL(p, out.wallType3, 64, "wall mesh 3");
  PARSE_STRING_OR_NULL(p, out.wallType4, 64, "wall mesh 4");
  PARSE_STRING_OR_NULL(p, out.wallType5, 64, "wall mesh 5");
  PARSE_STRING_OR_NULL(p, out.wallType6, 64, "wall mesh 6");
  PARSE_STRING_OR_NULL(p, out.entranceType, 64, "entrance mesh");
  PARSE_STRING_OR_NULL(p, out.exitType, 64, "exit mesh");
  PARSE_STRING_OR_NULL(p, out.exteriorWallType, 64, "exterior wall mesh");
  PARSE_OR_FAIL(parseFloat, out.wallRot, "maze wall rotation");
  PARSE_STRING_OR_NULL(p, out.layoutName, 64, "layout name");
  return p;
}
static bool parseMazeLayoutRow(const std::string& line, std::vector<bool>& outRow) {
  outRow.clear();
  for (char c : line) {
    if (c == '.') outRow.push_back(true);
    else if (c == 'X') outRow.push_back(false);
    else if (c != ' ') {
      fprintf(stderr, "[SceneLoader ERROR] Invalid character '%c' in maze layout line: \"%s\"\n", c, line.c_str());
      return false;
    }
  }
  return !outRow.empty();
}
bool parseMazeData(const unsigned char* p, ParsedMaze& maze) {
  PARSE_STRING_OR_FALSE(p, maze.layoutName, 64, "layout name");

  while (*p) {
    const char* lineStart = reinterpret_cast<const char*>(p);
    const char* lineEnd = reinterpret_cast<const char*>(skipToNextLine(p));
    size_t lineLen = lineEnd - lineStart;

    while (lineLen > 0 && (lineStart[lineLen - 1] == '\n' || lineStart[lineLen - 1] == '\r'))
      --lineLen;

    if (lineLen == 0) {
      p = reinterpret_cast<const unsigned char*>(lineEnd);
      continue;
    }

    std::string line(lineStart, lineLen);
    if (line == maze.layoutName)
      break;

    std::vector<bool> row;
    if (parseMazeLayoutRow(line, row))
      maze.layout.push_back(std::move(row));

    p = reinterpret_cast<const unsigned char*>(lineEnd);
  }

  if (maze.layout.empty()) {
    fprintf(stderr, "[SceneLoader ERROR] Maze layout was empty or not found: %s\n", maze.layoutName.c_str());
    return false;
  }

  return true;
}