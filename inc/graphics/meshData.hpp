#pragma once

#include "math/Vec4.hpp"
#include "math/vertex.hpp"
#include "core/colour.hpp"
#include <string>

/*
ModelDrawInfo
* Represents a single mesh (3d object) structure ' INDEXED' format
* Its layout is set up to match how the GPU sees the mesh, not how the file was, etc
* Its also storing the information we need to tell the GPU which model we want to draw.
*/
struct MeshData {
	std::string path;

	unsigned int VAOID, VertexBufferID, IndexBufferID;
	unsigned int VertexBuffer_Start_Index, IndexBuffer_Start_Index;
	unsigned int numVertices, numIndices, numTriangles;

	Vertex*       vertices;
	unsigned int* indices;

	bool hasNormals, hasColours, hasTexCoords;
	float minY, maxY;

	MeshData();
	~MeshData();

	MeshData(const MeshData&) = delete;
	MeshData& operator=(const MeshData&) = delete;

	MeshData(MeshData&& other) noexcept;
	MeshData& operator=(MeshData&& other) noexcept;
};