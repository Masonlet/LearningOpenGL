#include "utils/parser.hpp"
#include <cstdlib>  
#include <cstring>
#include <cstdio>

static inline bool isDelim(unsigned char c, bool comma = true) {
  return c == 0 || c == ' ' || c == '\t' || c == '\n' || c == '\r' || (comma && c == ',');
}

void skipToNextLine(const unsigned char*& p) {
  if (!p) return;
  while (*p && *p != '\n' && *p != '\r') ++p;

	if (*p == '\r') ++p; 
	if (*p == '\n') ++p; 
}
void skipToNextWord(const unsigned char*& p) {	
  if (!p) return;
  while (*p && !isDelim(*p)) ++p;
  while (*p && isDelim(*p)) ++p;
}
void skipWhitespace(const unsigned char*& p, bool skipComma) {
  while (p && *p && isDelim(*p, skipComma)) ++p;
}
void trimEOL(const unsigned char* p, const unsigned char*& end) {
  if (!p || !end) return;
  while (end > p && (end[-1] == '\n' || end[-1] == '\r')) --end;
}

bool parseUInt(const unsigned char*& p, unsigned int& out) {
  skipWhitespace(p);
  if (!p || *p == '\0' || *p < '0' || *p > '9') return false;

  out = 0;
  while (*p >= '0' && *p <= '9')
    out = out * 10 + (*p++ - '0');

  return true;
}
bool parseBinaryUInt(const unsigned char*& p, unsigned int& out) {
  if (!p) return false;

  out = (static_cast<unsigned int>(p[0]) << 24
    | static_cast<unsigned int>(p[1]) << 16
    | static_cast<unsigned int>(p[2]) << 8
    | static_cast<unsigned int>(p[3]));

  p += 4;
  return true;
}
bool parseBool(const unsigned char*& p, bool& out) {
  skipWhitespace(p);
  if (!p || *p == '\0') return false;

  unsigned char tok[6]{};
  if (!parseToken(p, tok, sizeof(tok))) return false;
  if (!p) return false;

  const char* str = reinterpret_cast<const char*>(tok);
  if (strcmp(str, "true") == 0 || strcmp(str, "on") == 0 || strcmp(str, "1") == 0)  out = true;
  else if (strcmp(str, "false") == 0 || strcmp(str, "off") == 0 || strcmp(str, "0") == 0) out = false;
  else return false;
  return true;
}
bool parseFloat(const unsigned char*& p, float& out) {
  skipWhitespace(p);
  if (!p || *p == '\0') return false;

  char* end = nullptr;
  const char* start = reinterpret_cast<const char*>(p);
  out = strtof(start, &end);
  if (end == start) return false;

  p = reinterpret_cast<const unsigned char*>(end);
  return true;
}

bool parseVec2(const unsigned char*& p, Vec2& out) {
  return parseFloat(p, out.x) && parseFloat(p, out.y);
}
bool parseVec3(const unsigned char*& p, Vec3& out) {
  return parseFloat(p, out.x) && parseFloat(p, out.y) && parseFloat(p, out.z);
}
bool parseVec4(const unsigned char*& p, Vec4& out) {
  return parseFloat(p, out.x) && parseFloat(p, out.y) && parseFloat(p, out.z) && parseFloat(p, out.w);
}

bool parseToken(const unsigned char*& p, unsigned char* out, const size_t maxLength) {
  if (!p || !out || maxLength == 0) return false;

  skipWhitespace(p);
  if (*p == '\0') return false;

  size_t i = 0;
  while (*p && !isDelim(*p) && i + 1 < maxLength) out[i++] = *p++;

  out[i] = '\0';
  return true;
}

static bool parseNumericColour(const unsigned char*& p, Vec4& out) {
	const unsigned char* original = p;
  if (parseVec4(p, out)) {
    if (out.x > 1.0f) out.x /= 255.0f;
    if (out.y > 1.0f) out.y /= 255.0f;
    if (out.z > 1.0f) out.z /= 255.0f;
    if (out.w > 1.0f) out.w /= 255.0f;
    return true;
  }

  p = original;
  Vec3 rgb;
  if (parseVec3(p, rgb)) {
    out = { rgb.x, rgb.y, rgb.z, 1.0f };
    if (out.x > 1) out.x /= 255.f;
    if (out.y > 1) out.y /= 255.f;
    if (out.z > 1) out.z /= 255.f;
    return true;
  }   

  p = original;
  return false;
}
static bool parseSpecialColour(const char* name, ColourMode& modeOut) {
  if      (strcmp(name, "Random")  == 0) modeOut = ColourMode::Random;
  else if (strcmp(name, "Rainbow") == 0) modeOut = ColourMode::VerticalGradient;
  else if (strcmp(name, "PLY")     == 0) modeOut = ColourMode::PLYColour;
  else return false;
  return true;
} 
static bool parseNamedColour(const unsigned char*& p, Vec4& colour, ColourMode& mode) {
  unsigned char input[64]{};
	if (!parseToken(p, input, sizeof(input))) return false;

  mode = ColourMode::Solid;
	const char* name = reinterpret_cast<const char*>(input);
  if      (strcmp(name, "Red") == 0)    colour = { 1.0f, 0.0f, 0.0f, 1.0f };
  else if (strcmp(name, "Green") == 0)  colour = { 0.0f, 1.0f, 0.0f, 1.0f };
  else if (strcmp(name, "Blue") == 0)   colour = { 0.0f, 0.0f, 1.0f, 1.0f };
  else if (strcmp(name, "Yellow") == 0) colour = { 1.0f, 1.0f, 0.0f, 1.0f };
  else if (strcmp(name, "White") == 0)  colour = { 1.0f, 1.0f, 1.0f, 1.0f };
  else if (strcmp(name, "Gray") == 0
        || strcmp(name, "Grey") == 0)   colour = { 0.5f, 0.5f, 0.5f, 1.0f };
  else return (parseSpecialColour(name, mode));
  return true;
}
bool parseColour(const unsigned char*& p, Vec4& colourOut, ColourMode& modeOut) { 
  colourOut = { 1.0f, 1.0f, 1.0f, 1.0f };
  modeOut = ColourMode::Solid;
  
  const unsigned char* original = p;
  if (parseNumericColour(p, colourOut)) return true;

	p = original;
  if (parseNamedColour(p, colourOut, modeOut)) return true;

	printf("[parseColour ERROR] Unknown colour format: %s\n", reinterpret_cast<const char*>(p));
  p = original;
  return false;
}
bool parseLightType(const unsigned char*& p, unsigned int& typeOut) {
	if (!p) return false;

  skipWhitespace(p);
  if (*p == '\0') return false;
    
  const unsigned char* original = p;
  if (parseUInt(p, typeOut)) return true;

  p = original;
  unsigned char typeName[64]{};
  parseToken(p, typeName, sizeof(typeName));
  if (!p) return false;

  if      (strcmp((char*)typeName, "Point")       == 0) typeOut = 0;
  else if (strcmp((char*)typeName, "Spot")        == 0) typeOut = 1;
  else if (strcmp((char*)typeName, "Directional") == 0) typeOut = 2;
  else {
    fprintf(stderr, "[parseLightType ERROR] Unknown light type: %s\n", reinterpret_cast<const char*>(typeName));
    return false;
  }

  return true;
}

bool parseCameraType(const unsigned char*& p, unsigned int& typeOut) {
	if (!p) return false;

  skipWhitespace(p);
  if (*p == '\0') return false;

  const unsigned char* original = p;
  if (parseUInt(p, typeOut)) return true;

  p = original;
  unsigned char typeName[64]{};   
  parseToken(p, typeName, sizeof(typeName));
  if (!p) return false;

  if      (strcmp((char*)typeName, "FreeCam")    == 0) typeOut = 0;
  else if (strcmp((char*)typeName, "DungeonCam") == 0) typeOut = 1;
  else if (strcmp((char*)typeName, "ModernCam")  == 0) typeOut = 2;
  else {
    fprintf(stderr, "[parseCameraType ERROR] Unknown camera type: %s\n", reinterpret_cast<const char*>(typeName));
		p = original;
    return false;
  }

	return true;
}