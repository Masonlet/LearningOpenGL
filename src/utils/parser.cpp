#include "utils/parser.hpp"

#include <string>
#include <cstring>

const unsigned char* skipToNextLine(const unsigned char* p) {
  if (!p) return nullptr;
  p = skipWhitespace(p);
  while (*p && *p != '\n') ++p;
  return (*p == '\n') ? p + 1 : p;
}
const unsigned char* skipToNextWord(const unsigned char* p) {	
  if (!p) return nullptr;
  while (*p != '\0' && *p != ' ' && *p != '\t' && *p != '\n' && *p != '\r') ++p;
  while (*p == ' ' || *p == '\t' || *p == '\n' || *p == '\r' || *p == ',') ++p;
  return p;
}
const unsigned char* skipWhitespace(const unsigned char* p) {
  if (!p) return nullptr;
  while (*p && (*p == ' ' || *p == '\t' || *p == '\n' || *p == '\r')) ++p;
  return p;
}

const unsigned char* parseToken(const unsigned char* p, unsigned char* out, const size_t maxLength) {
  if (maxLength == 0) return nullptr;
  p = skipWhitespace(p);
  if (!p || *p == '\0') return nullptr;

  size_t i = 0;
  while (*p && *p != ' ' && *p != '\t' && *p != '\r' && *p != '\n' && *p != ',' && i < maxLength - 1)
    out[i++] = *p++;

  out[i] = '\0';

  // Clean up newline characters
  while (i > 0 && (out[i - 1] == '\n' || out[i - 1] == '\r')) 
    out[--i] = '\0';
    
  return skipWhitespace(p);
}
const unsigned char* parseFloat(const unsigned char* p, float& out) {
  p = skipWhitespace(p);
  if (!p || *p == '\0') return nullptr;

  char* end = nullptr;
  out = strtof(reinterpret_cast<const char*>(p), &end);

  if (reinterpret_cast<const char*>(p) == end) return nullptr;
  return reinterpret_cast<const unsigned char*>(end);
}

const unsigned char* parseStringUInt(const unsigned char* p, unsigned int& out) {
  p = skipWhitespace(p);
  if (!p || *p == '\0' || *p < '0' || *p > '9') return nullptr;

  out = 0;
  while (*p >= '0' && *p <= '9') 
    out = out * 10 + (*p++ - '0');

  return p;
}
const unsigned int parseBinaryUINT(const unsigned char* buffer) {
    return (static_cast<unsigned int>(buffer[0]) << 24 | 
      static_cast<unsigned int>(buffer[1]) << 16 |
      static_cast<unsigned int>(buffer[2]) << 8 |
      static_cast<unsigned int>(buffer[3]));
}

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

  while(*p){
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

const unsigned char* parseVec3(const unsigned char* p, Vec3& out) {
    if (!(p = parseFloat(p, out.x)) || 
        !(p = parseFloat(p, out.y)) || 
        !(p = parseFloat(p, out.z))) 
        return nullptr;
    
    if (*p == ',') ++p;
    return p;
}
const unsigned char* parseVec4(const unsigned char* p, Vec4& out) {
  if (!(p = parseFloat(p, out.x)) ||
    !(p = parseFloat(p, out.y)) ||
    !(p = parseFloat(p, out.z)) ||
    !(p = parseFloat(p, out.w)))
    return nullptr;

  if (*p == ',') ++p;
  return p;
}

static const unsigned char* parseNumericColour(const unsigned char* p, Vec4& colourOut) {
  Vec3 tempColour = { 1.0f, 1.0f, 1.0f };

  if (!(p = parseVec3(p, tempColour)))
    return nullptr;

  if (tempColour.x > 1.0f || tempColour.y > 1.0f || tempColour.z > 1.0f) {
    tempColour.x /= 255.0f;
    tempColour.y /= 255.0f;
    tempColour.z /= 255.0f;
  }

  colourOut = { tempColour, 0.0f };
  return p;
}
static const unsigned char* parseSpecialColour(const char* name, ColourMode& modeOut) {
    if (strcmp(name, "Random") == 0) {
      modeOut = ColourMode::Random;
      return reinterpret_cast<const unsigned char*>(name + strlen(name));
  }
    else if (strcmp(name, "Rainbow") == 0) {
      modeOut = ColourMode::VerticalGradient;
      return reinterpret_cast<const unsigned char*>(name + strlen(name));
  }

  return nullptr;
}
static const unsigned char* parseNamedColour(const unsigned char* p, Vec4& colour, ColourMode& mode) {
  char colourName[64]{};
  p = parseToken(p, (unsigned char*)colourName, sizeof(colourName));
  if (!p) return nullptr;

  Vec3 tempColour = { 1.0f, 1.0f, 1.0f };
  if (strcmp(colourName, "Red") == 0) tempColour = { 1.0f, 0.0f, 0.0f };
  else if (strcmp(colourName, "Green") == 0) tempColour = { 0.0f, 1.0f, 0.0f };
  else if (strcmp(colourName, "Blue") == 0) tempColour = { 0.0f, 0.0f, 1.0f };
  else if (strcmp(colourName, "Yellow") == 0) tempColour = { 1.0f, 1.0f, 0.0f };
  else if (strcmp(colourName, "White") == 0) tempColour = { 1.0f, 1.0f, 1.0f };
  else if (strcmp(colourName, "Gray") == 0 || strcmp(colourName, "Grey") == 0) tempColour = { 0.5f, 0.5f, 0.5f };
  else return parseSpecialColour(colourName, mode);

  colour = { tempColour, 0.0f };
  mode = ColourMode::Solid;
  return p;
}
const unsigned char* parseColour(const unsigned char* p, Vec4& colourOut, ColourMode& modeOut) {
  colourOut = { 1.0f, 1.0f, 1.0f, 0.0f };
  modeOut = ColourMode::PLYColour;

  const unsigned char* original = skipWhitespace(p);
  if (*original == '\0' || *original == '-'){
    p = original;
    return p;
  }

  if ((p = parseNumericColour(p, colourOut))) {
    modeOut = ColourMode::Solid;
    return p;
  }

  p = original;
  if ((p = parseNamedColour(p, colourOut, modeOut))) {
    return p;
  }

  return original;
}