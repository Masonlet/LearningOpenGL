#pragma once

#include "core/modelDrawInfo.hpp"
const unsigned char* parsePlyHeader(const unsigned char* p, unsigned int& numVerticesOut, unsigned int& numTrianglesOut, bool& hasNormalsOut, bool& hasColoursOut);
const unsigned char* parseVertices(ModelDrawInfo& drawInfo, const unsigned char* p);
const unsigned char* parseIndices(ModelDrawInfo& drawInfo, const unsigned char* p);