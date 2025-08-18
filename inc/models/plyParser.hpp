#pragma once

#include "graphics/meshData.hpp"
bool parsePlyMesh(const unsigned char* p, unsigned int size, MeshData& drawInfo);
const unsigned char* parsePlyHeader(const unsigned char* p, unsigned int& numVerticesOut, unsigned int& numTrianglesOut, bool& hasNormalsOut, bool& hasColoursOut);
const unsigned char* parseVertices(MeshData& drawInfo, const unsigned char* p);
const unsigned char* parseIndices(MeshData& drawInfo, const unsigned char* p);