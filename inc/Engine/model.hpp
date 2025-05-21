#pragma once

#include "Vec3.hpp"
#include "Vertex.hpp"
#include "Transform.hpp"

constexpr Vec3 DEFAULT_NORMAL{0.0f, 0.0f, 0.0f};
constexpr Vec3 DEFAULT_COLOUR{0.0f, 1.0f, 0.0f};

struct Mesh {
	Vertex* vertices{nullptr};
	unsigned int* indices{nullptr};

	unsigned int numIndicesToDraw{0}, numVerticesToDraw{0}, sizeOfVertexArrayInBytes{0};
	unsigned int vao{0}, vbo{0}, ebo{0};

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

	void createModelFromBuffers();
};