#pragma once

const int strEquals(const char* a, const char* b);

const char* skipToNextLine(const char* p);
const char* skipWhitespace(const char* p);

const char* parseToken(const char* p, char* out, const size_t maxLength);
const char* parseFloat(const char* p, float& out);
const char* parseUInt(const char* p, unsigned int& out);

const char* parseHeader(const char* p, unsigned int& numVerticesOut, unsigned int& numTrianglesOut);
