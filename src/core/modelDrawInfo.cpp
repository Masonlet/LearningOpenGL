#include "core/modelDrawInfo.hpp"
#include "math/constants.hpp"

ModelDrawInfo::ModelDrawInfo() {
  this->VAOID = 0;

  this->VertexBufferID = 0;
  this->VertexBuffer_Start_Index = 0;
  this->numVertices = 0;

  this->IndexBufferID = 0;
  this->IndexBuffer_Start_Index = 0;
  this->numIndices = 0;
  this->numTriangles = 0;

  this->vertices = nullptr;
  this->indices = nullptr;

  this->hasNormals = false;
  this->hasColours = false;

  this->minY = 0.0f;
	this->maxY = 0.0f;
}
ModelDrawInfo::~ModelDrawInfo() {
  if (vertices) {
    delete[] vertices;
    vertices = nullptr;
  }
  if (indices) {
    delete[] indices;
    indices = nullptr;
  }
}

ModelDrawInfo::ModelDrawInfo(ModelDrawInfo&& other) noexcept {
  *this = static_cast<ModelDrawInfo&&>(other);
}

ModelDrawInfo& ModelDrawInfo::operator=(ModelDrawInfo&& other) noexcept {
  if (this != &other) {
    delete[] vertices;
    delete[] indices;

    meshPath = other.meshPath;

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

		minY = other.minY;
		maxY = other.maxY;

    other.vertices = nullptr;
    other.indices = nullptr;
  }
  return *this;
}
