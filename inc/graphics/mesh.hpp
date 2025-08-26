#pragma once

#include "math/Vec4.hpp"
#include "math/vertex.hpp"
#include "objects/colour.hpp"
#include <string>

/*
ModelDrawInfo
* Represents a single mesh (3d object) structure ' INDEXED' format
* Its layout is set up to match how the GPU sees the mesh, not how the file was, etc
* Its also storing the information we need to tell the GPU which model we want to draw.
*/
struct Mesh {
  Vertex* vertices{ nullptr };
  unsigned int* indices{ nullptr };
  unsigned int numVertices{ 0 }, numIndices{ 0 }, numTriangles{ 0 };

  bool hasNormals{ false }, hasColours{ false }, hasTexCoords{ false };
  float minY{ 0.0f }, maxY{ 0.0 };

	unsigned int VAOID{ 0 }, VertexBufferID{ 0 }, IndexBufferID{ 0 };
	unsigned int VertexBuffer_Start_Index{ 0 }, IndexBuffer_Start_Index{ 0 };

  inline bool empty() const { return !vertices || !indices || numVertices == 0 || numIndices == 0; }

	Mesh() = default;
  ~Mesh() {
    if (vertices) { delete[] vertices; vertices = nullptr; }
    if (indices)  { delete[] indices;  indices  = nullptr; }
  }

	Mesh(const Mesh&) = delete;
	Mesh& operator=(const Mesh&) = delete;

  Mesh(Mesh&& other) noexcept { *this = static_cast<Mesh&&>(other); }
  Mesh& operator=(Mesh&& other) noexcept {
    if (this != &other) {
      delete[] vertices;
      delete[] indices;

      VAOID = other.VAOID;
      VertexBufferID = other.VertexBufferID;
      IndexBufferID = other.IndexBufferID;
      VertexBuffer_Start_Index = other.VertexBuffer_Start_Index;
      IndexBuffer_Start_Index = other.IndexBuffer_Start_Index;

      numVertices = other.numVertices;
      numIndices = other.numIndices;
      numTriangles = other.numTriangles;

      vertices = other.vertices;
      indices = other.indices;

      hasNormals = other.hasNormals;
      hasColours = other.hasColours;
      hasTexCoords = other.hasTexCoords;

      minY = other.minY;
      maxY = other.maxY;

      other.vertices = nullptr;
      other.indices = nullptr;
    }
    return *this;
  }
};