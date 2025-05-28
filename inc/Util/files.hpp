#pragma once

#include "vertex.hpp"
#include <string>

struct sPlyVertexXYZ {
	Vec3 pos;
};

struct sPlyTriangle {
	unsigned int numVertices;
	int v0;
	int v1;	
	int v2;
};

extern Vertex* vertices;
extern unsigned int* indices;

extern volatile unsigned int g_NumIndicesToDraw;
extern volatile unsigned int g_NumVerticesToDraw;
extern volatile unsigned int g_SizeOfVertexArrayInBytes;

bool loadFile(unsigned char*& bufferOut, size_t& sizeOut, const std::string& path);
