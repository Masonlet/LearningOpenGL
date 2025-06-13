#include "parser.hpp"
#include <string>
#include <cstring>

const unsigned char* skipToNextLine(const unsigned char* p) {
  if(!p) return nullptr;
  while (*p != '\n' && *p != '\0') ++p;
  return (*p == '\n') ? p + 1 : p;
}
const unsigned char* skipToNextWord(const unsigned char* p) {	
  if(!p) return nullptr;
  while (*p != '\0' && *p != ' ' && *p != '\t' && *p != '\n' && *p != '\r') ++p;
  while (*p == ' ' || *p == '\t' || *p == '\n' || *p == '\r' || *p == ',') ++p;
  return p;
}
const unsigned char* skipWhitespace(const unsigned char* p) {
  if(!p) return nullptr;
  while (*p == ' ' || *p == '\t' || *p == '\n' || *p == '\r') ++p;
  return p;
}

const unsigned char* parseToken(const unsigned char* p, unsigned char* out, const size_t maxLength) {
  p = skipWhitespace(p);
  if(!p) return nullptr;

  size_t i = 0;
  while (*p && *p != ' ' && *p != '\t' && *p != '\r' && *p != ',' && i < maxLength - 1)
    out[i++] = *p++;

  out[i] = '\0';
  while (i > 0 && (out[i - 1] == '\n' || out[i - 1] == '\r')) {
  out[--i] = '\0';
  }
  return p;
}
const unsigned char* parseFloat(const unsigned char* p, float& out) {
 p = skipWhitespace(p);
  if (!p || *p == '\0') return nullptr;

  char* end = nullptr;
  out = strtof(reinterpret_cast<const char*>(p), &end);

  if (reinterpret_cast<const char*>(p) == end) {
    return nullptr;
  }

  return reinterpret_cast<const unsigned char*>(end);}

const unsigned char* parseStringUInt(const unsigned char* p, unsigned int& out) {
  p = skipWhitespace(p);
  if (!p || *p == '\0' || *p < '0' || *p > '9') return nullptr;

  out = 0;
  while (*p >= '0' && *p <= '9')
    out = out * 10 + (*p++ - '0');

  return p;
}
const unsigned int parseBinaryUINT(unsigned char* buffer) {
  /* bit shifting <<
  * x 00000001 
  * << becomes,
  * x 00000010
  */

  /* bitwise or |
  * x	    00000011
  * y	    00000110
  * x | y	00000111
  */
  return (buffer[0] << 24 | buffer[1] << 16 | buffer[2] << 8 | buffer[3]);
}

const unsigned char* parseHeader(const unsigned char* p, unsigned int& numVerticesOut, unsigned int& numTrianglesOut, bool& hasNormalsOut, bool& hasColoursOut) {
  numVerticesOut = 0;
  numTrianglesOut = 0;
  hasNormalsOut = false;
  hasColoursOut = false;

  bool hasNx = false, hasNy = false, hasNz = false;
  bool hasRed = false, hasGreen = false, hasBlue = false;

  while(*p){
    const char* lineStart = reinterpret_cast<const char*>(p);
    const char* lineEnd = reinterpret_cast<const char*>(skipToNextLine(p));
    int lineLen = static_cast<int>(lineEnd - lineStart);

    const unsigned char* trimmed = skipWhitespace(reinterpret_cast<const unsigned char*>(lineStart));

    if (strncmp((const char*)trimmed, "ply", 3) == 0 ||
        strncmp((const char*)trimmed, "format", 6) == 0 ||
        strncmp((const char*)trimmed, "comment", 7) == 0) {
      // Acceptable metadata, skip
    }
    else if (strncmp((const char*)trimmed, "element", 7) == 0 && (trimmed[7] == ' ' || trimmed[7] == '\t')) {
      trimmed = skipWhitespace(trimmed + 7);

      if (strncmp((const char*)trimmed, "vertex", 6) == 0 && (trimmed[6] == ' ' || trimmed[6] == '\t')) {
        trimmed = skipWhitespace(trimmed + 6);
        trimmed = parseStringUInt(trimmed, numVerticesOut);
      } else if (strncmp((const char*)trimmed, "face", 4) == 0 && (trimmed[4] == ' ' || trimmed[4] == '\t')) {
        trimmed = skipWhitespace(trimmed + 4);
        trimmed = parseStringUInt(trimmed, numTrianglesOut);
      }
    }
    else if (strncmp((const char*)trimmed, "property", 8) == 0) {
      trimmed = skipWhitespace(trimmed + 8);

      char propertyType[32], propertyName[32];
      trimmed = parseToken(trimmed, (unsigned char*)propertyType, sizeof(propertyType));
      trimmed = parseToken(trimmed, (unsigned char*)propertyName, sizeof(propertyName));

      if (strcmp(propertyName, "nx") == 0 || strcmp(propertyName, "normal_x") == 0) 
        hasNx = true;
      if (strcmp(propertyName, "ny") == 0 || strcmp(propertyName, "normal_y") == 0) 
        hasNy = true;
      if (strcmp(propertyName, "nz") == 0 || strcmp(propertyName, "normal_z") == 0) 
        hasNz = true;
      else if (strcmp(propertyName, "red") == 0) 
        hasRed = true;
      else if (strcmp(propertyName, "green") == 0) 
        hasGreen = true;
      else if (strcmp(propertyName, "blue") == 0) 
        hasBlue = true;
      else if (strcmp(propertyName, "alpha") == 0 || strcmp(propertyName, "a") == 0)
        hasColoursOut = true; // alpha implies color present
    }
    else if (strncmp((const char*)trimmed, "end_header", 10) == 0) {
      hasNormalsOut = hasNx && hasNy && hasNz;
      hasColoursOut = hasRed && hasGreen && hasBlue;

      return skipToNextLine(trimmed); // jump to first vertex line
    }
    else {
      // Log only unexpected lines
      fprintf(stderr, "[Warning] %.*s\n", lineLen, lineStart);
    }

    p = reinterpret_cast<const unsigned char*>(lineEnd);
  }

  return nullptr; // failed to find end_header
}

const unsigned char* parseVec3(const unsigned char* p, Vec3& out) {
  if (!(p = parseFloat(p, out.x)) ||
      !(p = parseFloat(p, out.y)) ||
      !(p = parseFloat(p, out.z))) {
    return nullptr;
  }
  return p;
}
