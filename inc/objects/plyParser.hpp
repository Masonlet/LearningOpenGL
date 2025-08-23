#pragma once

#include "objects/meshData.hpp"

bool parsePlyMesh(const std::string& path, MeshData& drawInfo);
bool parsePlyHeader(const unsigned char*& p, unsigned int& numVerticesOut, unsigned int& numTrianglesOut, bool& hasNormalsOut, bool& hasColoursOut, bool& hasTexCoordsOut);
bool parseVertices(const unsigned char*& p, MeshData& drawInfo);
bool parseIndices(const unsigned char*& p, MeshData& drawInfo);