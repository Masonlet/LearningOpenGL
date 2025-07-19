#pragma once

#include "math/vec3.hpp"
#include "math/vec4.hpp"
#include "core/colour.hpp"

#ifndef PARSE_OR_FAIL
#define PARSE_OR_FAIL(parser, target, errorMsg) \
  if (!(p = parser(p, target))) { \
      fprintf(stderr, "[Parser ERROR]: Failed to parse %s\n", errorMsg); \
      return nullptr; \
  } \
  if (*p == ',') ++p;
#endif

#ifndef PARSE_OR_FALSE
#define PARSE_OR_FALSE(parser, target, errorMsg) \
  if (!(p = parser(p, target))) { \
      fprintf(stderr, "[Parser ERROR]: Failed to parse %s\n", errorMsg); \
      return false; \
  } \
  if (*p == ',') ++p;
#endif

#ifndef PARSE_STRING_OR_FAIL
#define PARSE_STRING_OR_FAIL(p, target, size, label) \
  do {\
    char temp[size]{}; \
    p = parseToken(p, reinterpret_cast<unsigned char*>(temp), size); \
    if (!p || strlen(temp) == 0) { \
      fprintf(stderr, "[Parser ERROR] Failed to parse %s\n", label); \
      return nullptr; \
    } \
    if (*p == ',') ++p; \
    target = temp; \
  } while (0)
#endif

#ifndef PARSE_STRING_OR_FALSE
#define PARSE_STRING_OR_FALSE(p, target, size, label) \
  do {\
    char temp[size]{}; \
    p = parseToken(p, reinterpret_cast<unsigned char*>(temp), size); \
    if (!p || strlen(temp) == 0) { \
      fprintf(stderr, "[Parser ERROR] Failed to parse %s\n", label); \
      return false; \
    } \
    if (*p == ',') ++p; \
    target = temp; \
  } while (0)
#endif

const unsigned char* skipToNextLine(const unsigned char* p);
const unsigned char* skipToNextWord(const unsigned char* p);
const unsigned char* skipWhitespace(const unsigned char* p);

const unsigned char* parseToken     (const unsigned char* p, unsigned char* out, const size_t maxLength);
const unsigned char* parseFloat     (const unsigned char* p, float& out);	
const unsigned char* parseStringUInt(const unsigned char* p, unsigned int& out);
const unsigned int   parseBinaryUINT(const unsigned char* buffer);

const unsigned char* parseVec3(const unsigned char* p, Vec3& out);
const unsigned char* parseVec4(const unsigned char* p, Vec4& out);

const unsigned char* parseColour(const unsigned char* p, Vec4& colourOut, ColourMode& modeOut);
const unsigned char* parseLightType(const unsigned char* p, unsigned int& typeOut);
