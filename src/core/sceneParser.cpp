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

const unsigned char* parseMaze(const unsigned char* p, ParsedMaze& out) {
  char wall[128]{}, floor[128]{};

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
  p = skipWhitespace(p);

  out.floorType = floor;
  out.wallType = wall;

  return p;
}
const unsigned char* parseMazeData(const unsigned char* p, ParsedMaze& out) {
  out.layout.clear();

  while (*p) {
    const char* lineStart = reinterpret_cast<const char*>(p);
    const char* lineEnd = reinterpret_cast<const char*>(skipToNextLine(p));
    size_t len = lineEnd - lineStart;

    while (len > 0 && (lineStart[len - 1] == '\n' || lineStart[len - 1] == '\r'))
      --len;
    if (len == 0) break;

    std::vector<bool> row;
    for (size_t i = 0; i < len; ++i) {
      char c = lineStart[i];
      if (c == 'X') row.push_back(true);
      else if (c == '.') row.push_back(false);
      else {
        fprintf(stderr, "[parseMaze ERROR] Unexpected char in maze layout: '%c'\n", c);
        return nullptr;
      }
    }

    out.layout.push_back(row);
    p = reinterpret_cast<const unsigned char*>(lineEnd);
  }

  return p;
}