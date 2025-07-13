#include "core/sceneParser.hpp"
#include "utils/parser.hpp"

const unsigned char* parseModel(const unsigned char* p, ParsedModel& out) {
  PARSE_STRING_OR_FAIL(p, out.meshName, 64, "mesh name");
  PARSE_STRING_OR_FAIL(p, out.path, 128, "path");

  Vec3 temp;
  PARSE_OR_FAIL(parseVec3, temp, "Failed to parse position");
  out.position = { temp, 0.0f };

  PARSE_OR_FAIL(parseVec3, out.rotation, "Failed to parse rotation");
  PARSE_OR_FAIL(parseVec3, out.scale, "Failed to parse scale");

  p = parseColour(p, out.colour, out.colourMode);

  return p;
}
const unsigned char* parseTriangle(const unsigned char* p, ParsedTriangle& out) {
  PARSE_STRING_OR_FAIL(p, out.meshName, 64, "mesh name");

  Vec3 temp;
  PARSE_OR_FAIL(parseVec3, temp, "Failed to parse position");
  out.transform.position = { temp, 0.0f };

  PARSE_OR_FAIL(parseVec3, out.transform.rotation, "Failed to parse rotation");
  PARSE_OR_FAIL(parseVec3, out.transform.scale, "Failed to parse triangle size");

  p = parseColour(p, out.colour, out.colourMode);

  return p;
}
const unsigned char* parseGrid(const unsigned char* p, ParsedGrid& out) {
  PARSE_OR_FAIL(parseStringUInt, out.layout.count, "Failed to parse cubeGrid count");
  PARSE_OR_FAIL(parseFloat, out.layout.spacing, "Failed to parse cubeGrid spacing");

  PARSE_OR_FAIL(parseVec3, out.layout.start, "Failed to parse cubeGrid start position");
  PARSE_OR_FAIL(parseVec3, out.layout.rotation, "Failed to parse cubeGrid rotation");
  PARSE_OR_FAIL(parseVec3, out.layout.scale, "Failed to parse cubeGrid scale");

  p = parseColour(p, out.colour, out.colourMode);

  return p;
} 
const unsigned char* parseLight(const unsigned char* p, ParsedLight& out) {
  PARSE_OR_FAIL(parseStringUInt, out.index, "Failed to parse light index");
  PARSE_OR_FAIL(parseVec4, out.position, "Failed to parse light position");
  PARSE_OR_FAIL(parseVec4, out.diffuse, "Failed to parse light diffuse");
  PARSE_OR_FAIL(parseVec4, out.atten, "Failed to parse light attenuation");
  PARSE_OR_FAIL(parseVec4, out.direction, "Failed to parse light direction");
  PARSE_OR_FAIL(parseVec4, out.param1, "Failed to parse light param1");
  PARSE_OR_FAIL(parseVec4, out.param2, "Failed to parse light param2");

  return p;
}

const unsigned char* parseMaze(const unsigned char* p, ParsedMaze& out) {
  out.layout.clear();

  PARSE_STRING_OR_FAIL(p, out.mazeName, 64, "maze name");
  PARSE_STRING_OR_FAIL(p, out.floorType, 64, "floor mesh");
  PARSE_STRING_OR_FAIL(p, out.wallType, 64, "wall mesh");
  PARSE_STRING_OR_FAIL(p, out.layoutName, 64, "layout name");

  return p;
}

static bool parseMazeLayoutRow(const std::string& line, std::vector<bool>& outRow) {
  outRow.clear();
  for (char c : line) {
    if (c == 'X') outRow.push_back(true);
    else if (c == '.') outRow.push_back(false);
    else if (c != ' ') {
      fprintf(stderr, "[SceneLoader ERROR] Invalid character '%c' in maze layout line: \"%s\"\n", c, line.c_str());
      return false;
    }
  }
  return !outRow.empty();
}
bool parseMazeData(const unsigned char* p, ParsedMaze& maze, Scene& scene) {
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
    if (line == maze.layoutName) return true;

    std::vector<bool> row;
    if (parseMazeLayoutRow(line, row))
      maze.layout.push_back(std::move(row));

    p = reinterpret_cast<const unsigned char*>(lineEnd);
  }

  return true;
}