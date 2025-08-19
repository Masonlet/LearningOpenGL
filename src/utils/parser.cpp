#include "utils/parser.hpp"
#include <cstdlib>  
#include <cstring>
#include <cstdio>

static inline bool isSpace(unsigned char c, bool comma = false) {
  return comma ?  c == ' ' || c == '\t' || c == '\n' || c == '\r' || c == ','
               :  c == ' ' || c == '\t' || c == '\n' || c == '\r';
}

static inline bool isDelim(unsigned char c) {
  return c == 0 || c == ',' || c == ' ' || c == '\t' || c == '\n' || c == '\r';
}

const unsigned char* skipToNextLine(const unsigned char* p) {
  if (!p) return nullptr;
  while (*p && *p != '\n') ++p;
  return (*p == '\n') ? p + 1 : p;
}
const unsigned char* skipToNextWord(const unsigned char* p) {	
  if (!p) return nullptr;
  while (*p && !isDelim(*p)) ++p;
  while (*p && isDelim(*p)) ++p;
  return p;
}
const unsigned char* skipWhitespace(const unsigned char* p, bool skipComma) {
  if (!p) return nullptr;
  while (*p && isSpace(*p, skipComma)) ++p;
  return p;
}

const unsigned char* trimEOL(const unsigned char* p, const unsigned char* e) {
  if (!p || !e) return nullptr;
  while (e > p && (e[-1] == '\n' || e[-1] == '\r')) --e;
  return e;
}

const unsigned char* parseToken(const unsigned char* p, unsigned char* out, const size_t maxLength) {
  if (maxLength == 0) return nullptr;

  p = skipWhitespace(p, true);
  if (!p || *p == '\0') return nullptr;

  size_t i = 0;
  while (*p && !isDelim(*p) && i + 1 < maxLength)
    out[i++] = *p++;

  out[i] = '\0';
  return p;
}
const unsigned char* parseFloat(const unsigned char* p, float& out) {
  p = skipWhitespace(p, true);
  if (!p || *p == '\0') return nullptr;

  char* end = nullptr;
  out = strtof(reinterpret_cast<const char*>(p), &end);

  if (reinterpret_cast<const char*>(p) == end) return nullptr;
  else                                         return reinterpret_cast<const unsigned char*>(end);
}

const unsigned char* parseUInt(const unsigned char* p, unsigned int& out) {
  p = skipWhitespace(p, true);
  if (!p || *p == '\0' || *p < '0' || *p > '9') return nullptr;

  out = 0;
  while (*p >= '0' && *p <= '9') 
    out = out * 10 + (*p++ - '0');

  return p;
}
const unsigned int parseBinaryUINT(const unsigned char* buffer) {
    return (static_cast<unsigned int>(buffer[0]) << 24 
          | static_cast<unsigned int>(buffer[1]) << 16 
          | static_cast<unsigned int>(buffer[2]) << 8 
          | static_cast<unsigned int>(buffer[3]));
}

const unsigned char* parseBool(const unsigned char* p, bool& out) {
  p = skipWhitespace(p, true);
  if (!p || *p == '\0') return nullptr;

  unsigned char tok[6]{}; 
  const unsigned char* q = parseToken(p, tok, sizeof(tok));
  if (!q) return nullptr;

	const char* str = reinterpret_cast<const char*>(tok);
  if      (strcmp(str, "true") == 0  || strcmp(str, "on")  == 0 || strcmp(str, "1") == 0) out = true;  
  else if (strcmp(str, "false") == 0 || strcmp(str, "off") == 0 || strcmp(str, "0") == 0) out = false; 
  else return nullptr;

  return q;
}

const unsigned char* parseVec2(const unsigned char* p, Vec2& out) {
  p = skipWhitespace(p, true);
  if (!(p = parseFloat(p, out.x))) return nullptr;
  if (!(p = parseFloat(p, out.y))) return nullptr;
  return p;
}
const unsigned char* parseVec3(const unsigned char* p, Vec3& out) {
	p = skipWhitespace(p, true);
  if (!(p = parseFloat(p, out.x))) return nullptr;
  if (!(p = parseFloat(p, out.y))) return nullptr;
  if (!(p = parseFloat(p, out.z))) return nullptr;
  return p;
}
const unsigned char* parseVec4(const unsigned char* p, Vec4& out) {
	p = skipWhitespace(p, true);
  if (!(p = parseFloat(p, out.x))) return nullptr; 
  if (!(p = parseFloat(p, out.y))) return nullptr; 
  if (!(p = parseFloat(p, out.z))) return nullptr;
  if (!(p = parseFloat(p, out.w))) return nullptr;
  return p;
}

static const unsigned char* parseNumericColour(const unsigned char* p, Vec4& colourOut) {
  if (!(p = parseVec4(p, colourOut))) return nullptr;

  if (colourOut.x > 1.0f || colourOut.y > 1.0f || colourOut.z > 1.0f || colourOut.w > 1.0f) {
    colourOut.x /= 255.0f;
    colourOut.y /= 255.0f;
    colourOut.z /= 255.0f;
    colourOut.w /= 255.0f;
  }

  return p;
}
static bool parseSpecialColour(const char* name, ColourMode& modeOut) {
  if      (strcmp(name, "Random")  == 0) modeOut = ColourMode::Random;
  else if (strcmp(name, "Rainbow") == 0) modeOut = ColourMode::VerticalGradient;
  else if (strcmp(name, "PLY")     == 0) modeOut = ColourMode::PLYColour;
  else return false;
  return true;
}
static const unsigned char* parseNamedColour(const unsigned char* p, Vec4& colour, ColourMode& mode) {
  char colourName[64]{};
  const unsigned char* tokenEnd = parseToken(p, (unsigned char*)colourName, sizeof(colourName));
  if (!tokenEnd) return nullptr;

  mode = ColourMode::Solid;
  if      (strcmp(colourName, "Red") == 0)    colour = { 1.0f, 0.0f, 0.0f, 1.0f };
  else if (strcmp(colourName, "Green") == 0)  colour = { 0.0f, 1.0f, 0.0f, 1.0f };
  else if (strcmp(colourName, "Blue") == 0)   colour = { 0.0f, 0.0f, 1.0f, 1.0f };
  else if (strcmp(colourName, "Yellow") == 0) colour = { 1.0f, 1.0f, 0.0f, 1.0f };
  else if (strcmp(colourName, "White") == 0)  colour = { 1.0f, 1.0f, 1.0f, 1.0f };
  else if (strcmp(colourName, "Gray") == 0
        || strcmp(colourName, "Grey") == 0)   colour = { 0.5f, 0.5f, 0.5f, 1.0f };
  else if (!parseSpecialColour(colourName, mode)) return nullptr;

  return tokenEnd;
}
const unsigned char* parseColour(const unsigned char* p, Vec4& colourOut, ColourMode& modeOut) {
  colourOut = { 1.0f, 1.0f, 1.0f, 1.0f };
  modeOut = ColourMode::Solid;

  const unsigned char* original = skipWhitespace(p, true);
  if (!original || *original == '\0' || *original == '-') return original;

  if (const unsigned char* q = parseNumericColour(original, colourOut)) return q;
  if (const unsigned char* q = parseNamedColour(original, colourOut, modeOut)) return q;
  return original;
}
const unsigned char* parseLightType(const unsigned char* p, unsigned int& typeOut) {
	if (!p) return nullptr;

  const unsigned char* original = skipWhitespace(p, true);
  if (*original == '\0' || *original == '-') {
    p = original;
    return p;
  }

  const unsigned char* tryNum = parseUInt(original, typeOut);
  if (tryNum) return tryNum;

  unsigned char typeName[64]{};
  p = parseToken(original, typeName, sizeof(typeName));
  if (!p) return nullptr;

  if      (strcmp((char*)typeName, "Point")       == 0) typeOut = 0;
  else if (strcmp((char*)typeName, "Spot")        == 0) typeOut = 1;
  else if (strcmp((char*)typeName, "Directional") == 0) typeOut = 2;
  else {
    fprintf(stderr, "[parseLightType ERROR] Unknown light type: %s\n", typeName);
    return nullptr;
  }

  return p;
}

const unsigned char* parseCameraType(const unsigned char* p, unsigned int& typeOut) {
	if (!p) return nullptr;

  const unsigned char* original = skipWhitespace(p, true);
  if (*original == '\0' || *original == '-') {
    p = original;
    return p;
  }

  const unsigned char* tryNum = parseUInt(original, typeOut);
  if (tryNum) return tryNum;

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

	return p;
}