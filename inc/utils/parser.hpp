#pragma once

#include "math/vec2.hpp"
#include "math/vec3.hpp"
#include "math/vec4.hpp"
#include "core/colour.hpp"

#ifndef PARSE_OR_FAIL
#define PARSE_OR_FAIL(parser, target, errorMsg) \
  if (!(parser(p, target))) { \
      fprintf(stderr, "[Parser ERROR]: Failed to parse %s\n", errorMsg); \
      return nullptr; \
  } 
#endif

#ifndef PARSE_OR_FALSE
#define PARSE_OR_FALSE(parser, target, errorMsg) \
  if (!(parser(p, target))) { \
      fprintf(stderr, "[Parser ERROR]: Failed to parse %s\n", errorMsg); \
      return false; \
  } 
#endif

#ifndef PARSE_OR_INVALID
#define PARSE_OR_INVALID(parser, target, errorMsg) \
    if (valid && !(parser(p, target))) { \
        if(errorMsg && *errorMsg != '\0') fprintf(stderr, "[Parser ERROR]: Failed to parse %s\n", errorMsg); \
        valid = false; \
    } 
#endif

#ifndef PARSE_STRING_OR_NULL
#define PARSE_STRING_OR_NULL(p, target, size, label) \
  do {\
    char temp[size]{}; \
    if (!parseToken(p, reinterpret_cast<unsigned char*>(temp), size) || strlen(temp) == 0) { \
      fprintf(stderr, "[Parser ERROR] Failed to parse %s\n", label); \
      return nullptr; \
    } \
    target = temp; \
  } while (0)
#endif

#ifndef PARSE_STRING_OR_FALSE
#define PARSE_STRING_OR_FALSE(p, target, size, label) \
  do {\
    char temp[size]{}; \
    if (!parseToken(p, reinterpret_cast<unsigned char*>(temp), size) || strlen(temp) == 0) { \
      fprintf(stderr, "[Parser ERROR] Failed to parse %s\n", label); \
      return false; \
    } \
    target = temp; \
  } while (0)
#endif

void skipToNextLine(const unsigned char*& p);
void skipToNextWord(const unsigned char*& p);
void skipWhitespace(const unsigned char*& p, bool comma = true);
void trimEOL       (const unsigned char* p, const unsigned char*& end);

bool parseUInt      (const unsigned char*& p, unsigned int& out);
bool parseBinaryUInt(const unsigned char*& p, unsigned int& out);
bool parseBool      (const unsigned char*& p, bool& out);
bool parseFloat     (const unsigned char*& p, float& out);
bool parseVec2      (const unsigned char*& p, Vec2& out);
bool parseVec3      (const unsigned char*& p, Vec3& out);
bool parseVec4      (const unsigned char*& p, Vec4& out);
bool parseToken     (const unsigned char*& p, unsigned char* out, const size_t maxLength);

bool parseColour    (const unsigned char*& p, Vec4& colourOut, ColourMode& modeOut);
bool parseLightType (const unsigned char*& p, unsigned int& typeOut);
bool parseCameraType(const unsigned char*& p, unsigned int& typeOut);