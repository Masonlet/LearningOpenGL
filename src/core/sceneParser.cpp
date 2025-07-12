#include "core/sceneParser.hpp"
#include "utils/parser.hpp"

const unsigned char* parseModel(const unsigned char* p, ParsedModel& out) {
  char meshName[64]{};
  p = parseToken(p, reinterpret_cast<unsigned char*>(meshName), sizeof(meshName));
  if (!p || strlen(meshName) == 0) {
    fprintf(stderr, "[createSceneFromName ERROR] failed to parse path\n");
    return nullptr;
  }
  if (*p == ',') ++p;
  out.meshName = meshName;

  char path[128]{};
  p = parseToken(p, reinterpret_cast<unsigned char*>(path), sizeof(path));
  if (!p || strlen(path) == 0) {
    fprintf(stderr, "[createSceneFromName ERROR] failed to parse path\n");
    return nullptr;
  }
  if (*p == ',') ++p;
  out.path = path;

  Vec3 temp;
  PARSE_OR_FAIL(parseVec3, temp, "Failed to parse position");
  out.position = { temp, 0.0f };

  PARSE_OR_FAIL(parseVec3, out.rotation, "Failed to parse rotation");
  PARSE_OR_FAIL(parseVec3, out.scale, "Failed to parse scale");

  p = parseColour(p, out.colour, out.colourMode);

  return p;
}
const unsigned char* parseTriangle(const unsigned char* p, ParsedTriangle& out) {
  char meshName[64]{};
  p = parseToken(p, reinterpret_cast<unsigned char*>(meshName), sizeof(meshName));
  if (!p || strlen(meshName) == 0) {
    fprintf(stderr, "[createSceneFromName ERROR] failed to parse path\n");
    return nullptr;
  }
  if (*p == ',') ++p;
  out.meshName = meshName;

  Vec3 temp;
  PARSE_OR_FAIL(parseVec3, temp, "Failed to parse position");
  out.transform.position = { temp, 0.0f };

  PARSE_OR_FAIL(parseVec3, out.transform.rotation, "Failed to parse rotation");
  PARSE_OR_FAIL(parseVec3, out.transform.scale, "Failed to parse triangle size");

  p = parseColour(p, out.colour, out.colourMode);

  return p;
}
const unsigned char* parseCubeGrid(const unsigned char* p, ParsedGrid& out) {
  PARSE_OR_FAIL(parseStringUInt, out.layout.count, "Failed to parse cubeGrid count");
  PARSE_OR_FAIL(parseFloat, out.layout.spacing, "Failed to parse cubeGrid spacing");

  PARSE_OR_FAIL(parseVec3, out.layout.start, "Failed to parse cubeGrid start position");
  PARSE_OR_FAIL(parseVec3, out.layout.rotation, "Failed to parse cubeGrid rotation");
  PARSE_OR_FAIL(parseVec3, out.layout.scale, "Failed to parse cubeGrid scale");

  p = parseColour(p, out.colour, out.colourMode);

  return p;
}
const unsigned char* parseSquareGrid(const unsigned char* p, ParsedGrid& out) {
  PARSE_OR_FAIL(parseStringUInt, out.layout.count, "Failed to parse squareGrid count");
  PARSE_OR_FAIL(parseFloat, out.layout.spacing, "Failed to parse squareGrid spacing");

  PARSE_OR_FAIL(parseVec3, out.layout.start, "Failed to parse squareGrid start position");
  PARSE_OR_FAIL(parseVec3, out.layout.rotation, "Failed to parse squareGrid rotation");
  PARSE_OR_FAIL(parseVec3, out.layout.scale, "Failed to parse squareGrid size");

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

const unsigned char* parseMazeHeader(const unsigned char* p, ParsedMaze& out) {
  char mazeName[64]{}, floor[128]{}, wall[128]{}, layoutName[64]{};

  p = parseToken(p, reinterpret_cast<unsigned char*>(mazeName), sizeof(mazeName));
  if (!p || strlen(mazeName) == 0) {
    fprintf(stderr, "[parseMazeHeader ERROR] Failed to parse maze name\n");
    return nullptr;
  }
  if (*p == ',') ++p;

  p = parseToken(p, reinterpret_cast<unsigned char*>(floor), sizeof(floor));
  if (!p || strlen(floor) == 0) {
    fprintf(stderr, "[parseMaze ERROR] Failed to parse floor mesh path\n");
    return nullptr;
  }
  if (*p == ',') ++p;

  p = parseToken(p, reinterpret_cast<unsigned char*>(wall), sizeof(wall));
  if (!p || strlen(wall) == 0) {
    fprintf(stderr, "[parseMaze ERROR] Failed to parse wall mesh path\n");
    return nullptr;
  }
  if (*p == ',') ++p;

  p = parseToken(p, reinterpret_cast<unsigned char*>(layoutName), sizeof(layoutName));
  if (!p || strlen(layoutName) == 0) {
    fprintf(stderr, "[parseMazeHeader ERROR] Failed to parse maze layout name\n");
    return nullptr;
  }

  out.layout.clear();
  out.mazeName = mazeName;
  out.floorType = floor;
  out.wallType = wall;
  out.layoutName = layoutName;

  return p;
}
const unsigned char* parseMazeData(const unsigned char* p, ParsedMaze& maze, Scene& scene) {
  char layoutName[64]{};
  p = parseToken(p, reinterpret_cast<unsigned char*>(layoutName), sizeof(layoutName));
  if (!p || strlen(layoutName) == 0) {
    fprintf(stderr, "[SceneLoader ERROR] Maze layout name mismatch: expected '%s', got '%s'\n", maze.layoutName.c_str(), layoutName);
    return nullptr;
  }

  if (maze.layoutName != layoutName) {
    fprintf(stderr, "[SceneLoader ERROR] Maze layout name mismatch: expected '%s', got '%s'\n",
      maze.layoutName.c_str(), layoutName);
    return nullptr;
  }

  p = skipWhitespace(p);

  while (*p && *p != '\0') {
    const char* lineStart = reinterpret_cast<const char*>(p);
    const char* lineEnd = reinterpret_cast<const char*>(skipToNextLine(p));
    size_t lineLen = lineEnd - lineStart;

    while (lineLen > 0 && (lineStart[lineLen - 1] == '\n' || lineStart[lineLen - 1] == '\r'))
      --lineLen;

    if (lineLen == 0)
      return reinterpret_cast<const unsigned char*>(lineEnd);

    std::string line(lineStart, lineLen);

    if (line == maze.layoutName) 
      return reinterpret_cast<const unsigned char*>(lineEnd);

    std::vector<bool> row;
    for (char c : line) {
      if (c == 'X') row.push_back(true);
      else if (c == '.') row.push_back(false);
    }

    if (!row.empty())
      maze.layout.push_back(std::move(row));

    p = reinterpret_cast<const unsigned char*>(lineEnd);
  }

  fprintf(stderr, "[SceneLoader ERROR] Unexpected end of input while parsing maze layout\n");
  return nullptr;
}