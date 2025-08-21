#include "models/meshData.hpp"
#include "math/constants.hpp"

MeshData::~MeshData() {
  if (vertices) {
    delete[] vertices;
    vertices = nullptr;
  }
  if (indices) {
    delete[] indices;
    indices = nullptr;
  }
}

MeshData::MeshData(MeshData&& other) noexcept {
  *this = static_cast<MeshData&&>(other);
}

MeshData& MeshData::operator=(MeshData&& other) noexcept {
  if (this != &other) {
    delete[] vertices;
    delete[] indices;

    path = other.path;

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
