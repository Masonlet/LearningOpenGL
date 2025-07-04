#pragma once

#include "vec3.hpp"

#include <cstddef>

const unsigned int strEquals(const unsigned char* a, const char* b);

const unsigned char* skipToNextLine(const unsigned char* p);
const unsigned char* skipToNextWord(const unsigned char* p);
const unsigned char* skipWhitespace(const unsigned char* p);

const unsigned char* parseToken(const unsigned char* p, unsigned char* out, const size_t maxLength);
const unsigned char* parseFloat(const unsigned char* p, float& out);

const unsigned char* parseStringUInt(const unsigned char* p, unsigned int& out);
const unsigned int parseBinaryUINT(unsigned char* buffer);

const unsigned char* parseHeader(const unsigned char* p, unsigned int& numVerticesOut, unsigned int& numTrianglesOut, bool& hasNormalsOut, bool& hasColoursOut);

const unsigned char* parseVec3(const unsigned char* p, Vec3& out);
