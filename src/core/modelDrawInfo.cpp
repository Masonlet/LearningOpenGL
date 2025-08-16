#include "core/modelDrawInfo.hpp"
#include "math/constants.hpp"

ModelDrawInfo::ModelDrawInfo() {
  this->VAO_ID = 0;

  this->VertexBufferID = 0;
  this->VertexBuffer_Start_Index = 0;
  this->numVertices = 0;

  this->IndexBufferID = 0;
  this->IndexBuffer_Start_Index = 0;
  this->numIndices = 0;
  this->numTriangles = 0;

  this->vertices = nullptr;
  this->indices = nullptr;

  this->colour = DEFAULT_COLOUR;

  this->hasNormals = false;
  this->hasColours = false;

  this->modelMatrix = Mat4::identity();
  this->colourMode = ColourMode::Solid;
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
  *this = std::move(other);
}

ModelDrawInfo& ModelDrawInfo::operator=(ModelDrawInfo&& other) noexcept {
  if (this != &other) {
    delete[] vertices;
    delete[] indices;

    meshPath = std::move(other.meshPath);
    VAO_ID = other.VAO_ID;
    VertexBufferID = other.VertexBufferID;
    IndexBufferID = other.IndexBufferID;
    VertexBuffer_Start_Index = other.VertexBuffer_Start_Index;
    IndexBuffer_Start_Index = other.IndexBuffer_Start_Index;
    numVertices = other.numVertices;
    numIndices = other.numIndices;
    numTriangles = other.numTriangles;
    vertices = other.vertices;
    indices = other.indices;
    colour = other.colour;
    hasNormals = other.hasNormals;
    hasColours = other.hasColours;
    colourMode = other.colourMode;
    modelMatrix = other.modelMatrix;

    other.vertices = nullptr;
    other.indices = nullptr;
  }
  return *this;
}
