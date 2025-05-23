#pragma once

#include "Vec3.hpp"
#include "Vertex.hpp"
#include "Transform.hpp"

constexpr Vec3 DEFAULT_NORMAL{0.0f, 0.0f, 0.0f};
constexpr Vec3 DEFAULT_COLOUR{0.0f, 1.0f, 0.0f};

struct Mesh {
	Vertex* vertices;
	unsigned int* indices;

	unsigned int numIndicesToDraw, numVerticesToDraw, sizeOfVertexArrayInBytes;
	unsigned int vao, vbo, ebo;

	Mesh();
	~Mesh();

	bool createPLY(const char* path, const bool hasNormals);
	bool allocate(unsigned int vertexCount, unsigned int triangleCount);
	void upload();
};

struct Model {
	Mesh* mesh;
	Transform transform;
		
	Model();
	~Model();

	Model(const Model& b) = delete;
	Model& operator=(const Model& b) = delete;
};