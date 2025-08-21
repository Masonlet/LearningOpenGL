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
	std::string path{};

	unsigned int VAOID{ 0 }, VertexBufferID{ 0 }, IndexBufferID{ 0 };
	unsigned int VertexBuffer_Start_Index{ 0 }, IndexBuffer_Start_Index{ 0 };
	unsigned int numVertices{ 0 }, numIndices{ 0 }, numTriangles{ 0 };

	Vertex* vertices { nullptr };
	unsigned int* indices{ nullptr };

	bool hasNormals{ false }, hasColours{ false }, hasTexCoords{ false };
	float minY{ 0.0f }, maxY{ 0.0 };

	MeshData() = default;
	~MeshData();

	MeshData(const MeshData&) = delete;
	MeshData& operator=(const MeshData&) = delete;

	MeshData(MeshData&& other) noexcept;
	MeshData& operator=(MeshData&& other) noexcept;
};