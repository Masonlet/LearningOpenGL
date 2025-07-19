#include "utils/plyParser.hpp"
#include "utils/parser.hpp"

#include <string>

static const unsigned char* parsePlyElementLine(const unsigned char* p, unsigned int& verticesOut, unsigned int& trianglesOut) {
  const unsigned char* trimmed = skipWhitespace(p + 7);

  if (strncmp((const char*)trimmed, "vertex", 6) == 0 && (trimmed[6] == ' ' || trimmed[6] == '\t')) {
    trimmed = skipWhitespace(trimmed + 6);
    return parseStringUInt(trimmed, verticesOut);
  }
  else if (strncmp((const char*)trimmed, "face", 4) == 0 && (trimmed[4] == ' ' || trimmed[4] == '\t')) {
    trimmed = skipWhitespace(trimmed + 4);
    return parseStringUInt(trimmed, trianglesOut);
  }

  return p;
}

static const unsigned char* parsePlyPropertyLine(const unsigned char* p, bool& hasNx, bool& hasNy, bool& hasNz, bool& hasR, bool& hasG, bool& hasB) {
  const unsigned char* trimmed = skipWhitespace(p + 8);

  char temp[32]{};
  if (!(trimmed = parseToken(trimmed, (unsigned char*)temp, sizeof(temp)))) {
    fprintf(stderr, "[parsePlyHeader ERROR] Failed to parse property list count type, type: %s\n", temp);
    return nullptr;
  }

  if (strcmp(temp, "list") == 0) {
    /*
     1 = Count Type
     2 = Value Type
     3 = Property Name
    */

    char property[3][32]{};
    for (int i = 0; i < 3; ++i) {
      if (!(trimmed = parseToken(trimmed, (unsigned char*)property[i], sizeof(property[i])))) {
        fprintf(stderr, "[parsePlyHeader ERROR] Failed to parse property list number %d\n", i);
        return nullptr;
      }
    }
    return trimmed;
  }

  char propertyName[32]{};
  trimmed = parseToken(trimmed, (unsigned char*)propertyName, sizeof(propertyName));

  if (strcmp(propertyName, "nx") == 0 || strcmp(propertyName, "normal_x") == 0) hasNx = true;
  else if (strcmp(propertyName, "ny") == 0 || strcmp(propertyName, "normal_y") == 0) hasNy = true;
  else if (strcmp(propertyName, "nz") == 0 || strcmp(propertyName, "normal_z") == 0) hasNz = true;
  else if (strcmp(propertyName, "red") == 0) hasR = true;
  else if (strcmp(propertyName, "green") == 0) hasG = true;
  else if (strcmp(propertyName, "blue") == 0) hasB = true;
  else if (strcmp(propertyName, "alpha") == 0 || strcmp(propertyName, "a") == 0) {} // Alpha for future

  return trimmed;
}

static bool checkPlyNormalsAndColors(bool hasNx, bool hasNy, bool hasNz, bool hasR, bool hasG, bool hasB, bool& hasNormals, bool& hasColors) {
  hasNormals = hasNx && hasNy && hasNz;
  hasColors = hasR && hasG && hasB;
  return true;
}

static const unsigned char* findPlyHeaderEnd(const unsigned char* p) {
  while (*p) {
    const unsigned char* trimmed = skipWhitespace(p);
    if (strncmp((const char*)trimmed, "end_header", 10) == 0) {
      return skipToNextLine(trimmed);
    }
    p = skipToNextLine(p);
  }
  return nullptr;
}

const unsigned char* parsePlyHeader(const unsigned char* p, unsigned int& numVerticesOut, unsigned int& numTrianglesOut, bool& hasNormalsOut, bool& hasColoursOut) {
  if (!p) return nullptr;

  numVerticesOut = 0;
  numTrianglesOut = 0;
  hasNormalsOut = false;
  hasColoursOut = false;

  bool hasNx = false, hasNy = false, hasNz = false;
  bool hasRed = false, hasGreen = false, hasBlue = false;

  while (*p) {
    const char* lineStart = reinterpret_cast<const char*>(p);
    const char* lineEnd = reinterpret_cast<const char*>(skipToNextLine(p));
    int lineLen = static_cast<int>(lineEnd - lineStart);

    const unsigned char* trimmed = skipWhitespace(reinterpret_cast<const unsigned char*>(lineStart));

    if (strncmp((const char*)trimmed, "element", 7) == 0 && (trimmed[7] == ' ' || trimmed[7] == '\t'))
      parsePlyElementLine(trimmed, numVerticesOut, numTrianglesOut);

    else if (strncmp((const char*)trimmed, "property", 8) == 0) {
      parsePlyPropertyLine(trimmed, hasNx, hasNy, hasNz, hasRed, hasGreen, hasBlue);
    }
    else if (strncmp((const char*)trimmed, "end_header", 10) == 0) {
      checkPlyNormalsAndColors(hasNx, hasNy, hasNz, hasRed, hasGreen, hasBlue, hasNormalsOut, hasColoursOut);
      return skipToNextLine(trimmed); // jump to first vertex line
    }
    // Log only unexpected lines
    else if (!(strncmp((const char*)trimmed, "ply", 3) == 0) &&
      !(strncmp((const char*)trimmed, "format", 6) == 0) &&
      !(strncmp((const char*)trimmed, "comment", 7) == 0))
      fprintf(stderr, "[Warning] %.*s\n", lineLen, lineStart);

    p = reinterpret_cast<const unsigned char*>(lineEnd);
  }

  return nullptr; // failed to find end_header
}