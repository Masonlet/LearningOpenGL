#pragma once

#include "starletmath/Vec4.hpp"
#include "starletmath/vertex.hpp"
#include "scene/objects/colour.hpp"
#include <string>

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