#include <glad/glad.h>

#include "utils/parser.hpp"

#include <string>

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
    return tryNum;
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

  return (*p == ',') ? ++p : p;
}

const unsigned char* parseCameraType(const unsigned char* p, unsigned int& typeOut) {
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
    return tryNum;
  }

  unsigned char typeName[64]{}; 
  p = parseToken(original, typeName, sizeof(typeName));
  if (!p) return nullptr;

  if (strcmp((char*)typeName, "FreeCam") == 0) typeOut = 0;
  else if (strcmp((char*)typeName, "DungeonCam") == 0) typeOut = 1;
  else if (strcmp((char*)typeName, "ModernCam") == 0) typeOut = 2;
  else {
    fprintf(stderr, "[parseCameraType ERROR] Unknown camera type: %s\n", typeName);
    return nullptr;
  }

	return (*p == ',') ? ++p : p;
}