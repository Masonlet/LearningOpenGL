#pragma once

#include "models/meshData.hpp"

bool parsePlyMesh(const unsigned char*& p, unsigned int size, MeshData& drawInfo);
bool parsePlyHeader(const unsigned char*& p, unsigned int& numVerticesOut, unsigned int& numTrianglesOut, bool& hasNormalsOut, bool& hasColoursOut, bool& hasTexCoordsOut);
bool parseVertices(const unsigned char*& p, MeshData& drawInfo);
bool parseIndices(const unsigned char*& p, MeshData& drawInfo);