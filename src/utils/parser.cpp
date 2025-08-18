#include <glad/glad.h>
#include "utils/parser.hpp"
#include <cstdlib>  
#include <cstring>
#include <cstdio>

const unsigned char* skipToNextLine(const unsigned char* p) {
  if (!p) return nullptr;
  while (*p && *p != '\n') ++p;
  return (*p == '\n') ? p + 1 : p;
}
const unsigned char* skipToNextWord(const unsigned char* p) {	
  if (!p) return nullptr;
  while (*p != '\0' && *p != ' ' && *p != '\t' && *p != '\n' && *p != '\r' && *p != ',') ++p;
  while (*p == ' ' || *p == '\t' || *p == '\n' || *p == '\r' || *p == ',') ++p;
  return p;
}
const unsigned char* skipWhitespace(const unsigned char* p) {
  if (!p) return nullptr;
  while (*p == ' ' || *p == '\t') ++p;
  return p;
}
const unsigned char* skipWhitespaceComma(const unsigned char* p) {
  if (!p) return nullptr;
  while (*p == ' ' || *p == '\t') ++p;
  return (*p == ',') ? p + 1 : p;
}

const unsigned char* trimEOL(const unsigned char* p, const unsigned char* e) {
  if (!p || !e) return nullptr;
  while (e > p && (e[-1] == '\n' || e[-1] == '\r')) --e;
  return e;
}

const unsigned char* parseToken(const unsigned char* p, unsigned char* out, const size_t maxLength) {
  if (maxLength == 0) return nullptr;
  p = skipWhitespace(p);
  if (!p || *p == '\0') return nullptr;

  size_t i = 0;
  while (*p && *p != ' ' && *p != '\t' && *p != '\r' && *p != '\n' && *p != ',' && i < maxLength - 1)
    out[i++] = *p++;

  out[i] = '\0';

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

const unsigned char* parseUInt(const unsigned char* p, unsigned int& out) {
  p = skipWhitespace(p);
  if (!p || *p == '\0' || *p < '0' || *p > '9') return nullptr;

  out = 0;
  while (*p >= '0' && *p <= '9') 
    out = out * 10 + (*p++ - '0');

  return skipWhitespaceComma(p);
}
const unsigned int parseBinaryUINT(const unsigned char* buffer) {
    return (static_cast<unsigned int>(buffer[0]) << 24 
          | static_cast<unsigned int>(buffer[1]) << 16 
          | static_cast<unsigned int>(buffer[2]) << 8 
          | static_cast<unsigned int>(buffer[3]));
}

static inline bool isDelim(unsigned char c) {
  return c == 0 || c == ',' || c == ' ' || c == '\t' || c == '\n' || c == '\r';
}
const unsigned char* parseBool(const unsigned char* p, bool& out) {
  p = skipWhitespace(p);
  if (!p || *p == '\0') return nullptr;

  if (strncmp(reinterpret_cast<const char*>(p), "true", 4) == 0 && isDelim(p[4])) {
    out = true;
    p += 4;
    return skipWhitespaceComma(p);
  }
  if (strncmp(reinterpret_cast<const char*>(p), "false", 5) == 0 && isDelim(p[5])) {
    out = false;
    p += 5;
    return skipWhitespaceComma(p);
  }
  if (strncmp(reinterpret_cast<const char*>(p), "on", 2) == 0 && isDelim(p[2])) {
    out = true;
    p += 2;
    return skipWhitespaceComma(p);
  }
  if (strncmp(reinterpret_cast<const char*>(p), "off", 3) == 0 && isDelim(p[3])) {
    out = false;
    p += 3;
    return skipWhitespaceComma(p);
  }
  if (*p == '1' && isDelim(p[1])) {
    out = true;
    ++p;
    return skipWhitespaceComma(p);
  }
  if (*p == '0' && isDelim(p[1])) {
    out = false;
    ++p;
    return skipWhitespaceComma(p);
  }

  return nullptr;
}

const unsigned char* parseVec3(const unsigned char* p, Vec3& out) {
  if (!(p = parseFloat(p, out.x))) return nullptr;
  p = skipToNextWord(p);
  if (!(p = parseFloat(p, out.y))) return nullptr;
  p = skipToNextWord(p);
  if (!(p = parseFloat(p, out.z))) return nullptr;
    
  return skipWhitespaceComma(p);
}
const unsigned char* parseVec4(const unsigned char* p, Vec4& out) {
  if (!(p = parseFloat(p, out.x))) return nullptr; 
  p = skipToNextWord(p);
  if (!(p = parseFloat(p, out.y))) return nullptr; 
  p = skipToNextWord(p);
  if (!(p = parseFloat(p, out.z))) return nullptr;
  p = skipToNextWord(p);
  if (!(p = parseFloat(p, out.w))) return nullptr;

  return skipWhitespaceComma(p);
}

static const unsigned char* parseNumericColour(const unsigned char* p, Vec4& colourOut) {
  Vec4 tempColour = { 1.0f, 1.0f, 1.0f, 1.0f };

  if (!(p = parseVec4(p, tempColour)))
    return nullptr;

  if (tempColour.x > 1.0f || tempColour.y > 1.0f || tempColour.z > 1.0f || tempColour.w > 1.0f) {
    tempColour.x /= 255.0f;
    tempColour.y /= 255.0f;
    tempColour.z /= 255.0f;
    tempColour.w /= 255.0f;
  }

  colourOut = tempColour;
  return p;
}
static const unsigned char* parseSpecialColour(const char* name, ColourMode& modeOut) {
  if (strcmp(name, "Random") == 0) modeOut = ColourMode::Random;
  else if (strcmp(name, "Rainbow") == 0) modeOut = ColourMode::VerticalGradient;
  else if (strcmp(name, "PLY") == 0) modeOut = ColourMode::PLYColour;
  else return nullptr;
  return reinterpret_cast<const unsigned char*>(name + strlen(name));
}
static const unsigned char* parseNamedColour(const unsigned char* p, Vec4& colour, ColourMode& mode) {
  char colourName[64]{};
  const unsigned char* tokenEnd = parseToken(p, (unsigned char*)colourName, sizeof(colourName));
  if (!tokenEnd) return nullptr;

  Vec3 tempColour = { 1.0f, 1.0f, 1.0f };

  if (strcmp(colourName, "Red") == 0) tempColour = { 1.0f, 0.0f, 0.0f };
  else if (strcmp(colourName, "Green") == 0) tempColour = { 0.0f, 1.0f, 0.0f };
  else if (strcmp(colourName, "Blue") == 0) tempColour = { 0.0f, 0.0f, 1.0f };
  else if (strcmp(colourName, "Yellow") == 0) tempColour = { 1.0f, 1.0f, 0.0f };
  else if (strcmp(colourName, "White") == 0) tempColour = { 1.0f, 1.0f, 1.0f };
  else if (strcmp(colourName, "Gray") == 0 || strcmp(colourName, "Grey") == 0) tempColour = { 0.5f, 0.5f, 0.5f };
  else {  
    if (!parseSpecialColour(colourName, mode)) return nullptr;
    return skipWhitespaceComma(tokenEnd);
  }

  colour = { tempColour, 1.0f };
  mode = ColourMode::Solid;
  return skipWhitespaceComma(tokenEnd);
}
const unsigned char* parseColour(const unsigned char* p, Vec4& colourOut, ColourMode& modeOut) {
  colourOut = { 1.0f, 1.0f, 1.0f, 1.0f };
  modeOut = ColourMode::Solid;

  const unsigned char* original = skipWhitespace(p);
  if (*original == '\0' || *original == '-') return original;

  p = original;
  if ((p = parseNumericColour(p, colourOut))) return p;
 
  p = original;
  if ((p = parseNamedColour(p, colourOut, modeOut))) return p;

  return skipWhitespaceComma(original);
}
const unsigned char* parseLightType(const unsigned char* p, unsigned int& typeOut) {
  typeOut = 0;

  const unsigned char* original = p;
  if (*original == '\0' || *original == '-') {
    p = original;
    return p;
  }

  float typeNum = 0.0f;
  const unsigned char* tryNum = parseFloat(original, typeNum);
  if (tryNum) {
    typeOut = static_cast<unsigned int>(typeNum);
    return skipWhitespaceComma(tryNum);
  }

  unsigned char typeName[64]{};
  p = parseToken(original, typeName, sizeof(typeName));
  if (!p) return nullptr;

  if (strcmp((char*)typeName, "Point") == 0) typeOut = 0;
  else if (strcmp((char*)typeName, "Spot") == 0) typeOut = 1;
  else if (strcmp((char*)typeName, "Directional") == 0) typeOut = 2;
  else {
    fprintf(stderr, "[parseLightType ERROR] Unknown light type: %s\n", typeName);
    return nullptr;
  }

  return skipWhitespaceComma(p);
}

const unsigned char* parseCameraType(const unsigned char* p, unsigned int& typeOut) {
  typeOut = 0;

  const unsigned char* original = p;
  if (*original == '\0' || *original == '-') {
    p = original;
    return skipWhitespaceComma(p);
  }

  float typeNum = 0.0f;
  const unsigned char* tryNum = parseFloat(original, typeNum);
  if (tryNum) {
    typeOut = static_cast<unsigned int>(typeNum);
    return skipWhitespaceComma(tryNum);
  }

  unsigned char typeName[64]{};   
  p = parseToken(original, typeName, sizeof(typeName));
  if (!p) return nullptr;
  else if (strcmp((char*)typeName, "FreeCam")    == 0) typeOut = 0;
  else if (strcmp((char*)typeName, "DungeonCam") == 0) typeOut = 1;
  else if (strcmp((char*)typeName, "ModernCam")  == 0) typeOut = 2;
  else {
    fprintf(stderr, "[parseCameraType ERROR] Unknown camera type: %s\n", typeName);
    return nullptr;
  }

	return skipWhitespaceComma(p);
}