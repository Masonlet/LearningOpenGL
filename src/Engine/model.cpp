#include "model.hpp"
#include "init.hpp"
#include "shaders.hpp"
#include "files.hpp"
#include "parser.hpp"

#include <cstdio>
#include <cmath>

static _forceinline const unsigned char* parseMeshVertices(Mesh& mesh, const unsigned char* p, unsigned int numVertices, bool hasNormals = false) {
	for (unsigned int i{0}; i < numVertices; i++) {
		p = parseFloat(p, mesh.vertices[i].pos.x);
		p = parseFloat(p, mesh.vertices[i].pos.y);
		p = parseFloat(p, mesh.vertices[i].pos.z);
		if (hasNormals) {
			p = parseFloat(p, mesh.vertices[i].norm.x);
			p = parseFloat(p, mesh.vertices[i].norm.y);
			p = parseFloat(p, mesh.vertices[i].norm.z);
		} else {
			mesh.vertices[i].norm = DEFAULT_NORMAL;
		}
		p = skipToNextLine(p);

		mesh.vertices[i].col = DEFAULT_COLOUR;
	}

	return p;
}
static _forceinline const unsigned char* parseMeshIndices(Mesh& mesh, const unsigned char* p, unsigned int numTriangles) {
	for (unsigned int i = 0, b = 0; i < numTriangles; i++, b += 3) {
		unsigned int vertices{0};
		p = parseStringUInt(p, vertices);

		if (vertices != 3) {
			fprintf(stderr, "Error: Triangle %u has %u vertices (expected 3)\n", i, vertices);

			return nullptr;
		}

		p = parseStringUInt(p, mesh.indices[b + 0]);
		p = parseStringUInt(p, mesh.indices[b + 1]);
		p = parseStringUInt(p, mesh.indices[b + 2]);
		p = skipToNextLine(p);
	}

	return p;
}

Mesh::Mesh() : vertices{nullptr}, indices{nullptr}, numIndicesToDraw{0}, numVerticesToDraw{0}, sizeOfVertexArrayInBytes{0}, vao{0}, vbo{0}, ebo{0} {}
Mesh::~Mesh() {
	if (vertices) delete[] vertices;
	if (indices) delete[] indices;

	if (vao) glDeleteVertexArrays(1, &vao);
    if (vbo) glDeleteBuffers(1, &vbo);
    if (ebo) glDeleteBuffers(1, &ebo);

	vertices = nullptr;
	indices = nullptr;
	vao = vbo = ebo = 0;
}

void Mesh::upload() {
	if (vao && vbo && ebo) return;

	vbo = createBuffer(GL_ARRAY_BUFFER, sizeOfVertexArrayInBytes, vertices);
	ebo = createBuffer(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned int) * numIndicesToDraw, indices);
	vao = createArray(vbo, ebo);
}
bool Mesh::allocate(unsigned int vertexCount, unsigned int triangleCount) {
	sizeOfVertexArrayInBytes = sizeof(Vertex) * vertexCount;
	vertices = new Vertex[vertexCount];
	indices = new unsigned int[triangleCount * 3];

	if (!vertices || !indices) {
		fprintf(stderr, "Memory allocation failed\n");
		return false;
	}

	return true;
}
bool Mesh::createPLY(const char* path, const bool hasNormals) {
	unsigned char* buffer{};
	size_t discard;

	if (!loadFile(buffer, path, discard)) {
		fprintf(stderr, "Failed to load file: %s\n", path);
		return false;
	}
	
	unsigned int vertexCount{0}, triangleCount{0};
	const unsigned char* cursor = parseHeader(buffer, vertexCount, triangleCount);
	if (vertexCount == 0 || triangleCount == 0 || !cursor) {
		fprintf(stderr, "Invalid Number / Formatting of Vertices / Triangles\n");
		return false;
	}

	if (!allocate(vertexCount, triangleCount)) {
		return false;
	}

	cursor = parseMeshVertices(*this, cursor, vertexCount, hasNormals);

	if (!cursor) {
		fprintf(stderr, "Vertex data input failed\n");
		return false;
	}

	cursor = parseMeshIndices(*this, cursor, triangleCount);
	if (!cursor) {
		fprintf(stderr, "Face data input failed\n");
		return false;
	}

	numVerticesToDraw = vertexCount;
	numIndicesToDraw = triangleCount * 3;
	return true;
}

Model::Model() : mesh{nullptr} {}
Model::~Model() {
	delete mesh;
}

