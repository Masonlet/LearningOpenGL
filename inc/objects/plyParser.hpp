#pragma once

#include "objects/mesh.hpp"

bool parsePlyMesh(const std::string& path, Mesh& drawInfo);
bool parsePlyHeader(const unsigned char*& p, unsigned int& numVerticesOut, unsigned int& numTrianglesOut, bool& hasNormalsOut, bool& hasColoursOut, bool& hasTexCoordsOut);
bool parseVertices(const unsigned char*& p, Mesh& drawInfo);
bool parseIndices(const unsigned char*& p, Mesh& drawInfo);