#pragma once

#include "math/Vec4.hpp"
#include "math/mat4.hpp"
#include "math/vertex.hpp"
#include "core/colour.hpp"
#include <string>

/*
ModelDrawInfo
* Represents a single mesh (3d object) structure ' INDEXED' format
* Its layout is set up to match how the GPU sees the mesh, not how the file was, etc
* Its also storing the information we need to tell the GPU which model we want to draw.
*/
struct ModelDrawInfo {
	ModelDrawInfo();
	~ModelDrawInfo();

	ModelDrawInfo(const ModelDrawInfo&) = delete;
	ModelDrawInfo& operator=(const ModelDrawInfo&) = delete;

	ModelDrawInfo(ModelDrawInfo&& other) noexcept;
	ModelDrawInfo& operator=(ModelDrawInfo&& other) noexcept;

	std::string meshPath;

	unsigned int VAO_ID;

	unsigned int VertexBufferID, IndexBufferID;
	unsigned int VertexBuffer_Start_Index, IndexBuffer_Start_Index;
	unsigned int numVertices, numIndices, numTriangles;

	Vertex* vertices;
	unsigned int* indices;
	Vec4 colour;

	bool hasNormals, hasColours;

	ColourMode colourMode;
	Mat4 modelMatrix;
};