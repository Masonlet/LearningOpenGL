#include "models/primitives.hpp"

bool createTriangle(MeshManager* meshManager, const std::string& name, unsigned int shaderID, const Vec2& size, const Vec4& vertexColour) {
	MeshData info;
	info.path = name;
	info.numVertices = 3;
	info.numIndices = 3;
	info.numTriangles = 1;
	
	info.vertices = new Vertex[3];
	info.indices = new unsigned int[3];
	
	info.vertices[0].pos = { -0.5f * size.x, -0.5f * size.y, 0.0f, 1.0f };
	info.vertices[1].pos = {  0.5f * size.x, -0.5f * size.y, 0.0f, 1.0f };
	info.vertices[2].pos = {  0.0f,           0.5f * size.y, 0.0f, 1.0f };

	for (int i = 0; i < 3; ++i)
		info.vertices[i].col = vertexColour;

	info.indices[0] = 0;
	info.indices[1] = 1;
	info.indices[2] = 2;

	return meshManager->loadMeshPrimitive(info, shaderID);
}

bool createSquare(MeshManager* meshManager, const std::string& name, unsigned int shaderID, const Vec2& size) {
	MeshData info;
	info.path = name;
	info.numVertices = 6;
	info.numIndices = 6;
	info.numTriangles = 2;

	info.vertices = new Vertex[6];
	info.indices = new unsigned int[6];

	const float halfX = 0.5f * size.x;
	const float halfY = 0.5f * size.y;

	// First triangle
	info.vertices[0].pos = { -halfX, -halfY, 0.0f, 1.0f };
	info.vertices[1].pos = {  halfX, -halfY, 0.0f, 1.0f };
	info.vertices[2].pos = {  halfX,  halfY, 0.0f, 1.0f };

	// Second triangle
	info.vertices[3].pos = { -halfX, -halfY, 0.0f, 1.0f };
	info.vertices[4].pos = {  halfX,  halfY, 0.0f, 1.0f };
	info.vertices[5].pos = { -halfX,  halfY, 0.0f, 1.0f };

	for (int i = 0; i < 6; ++i) {
		info.vertices[i].col = { 1.0f, 1.0f, 1.0f, 1.0f };
		info.indices[i] = i;
	}

	return meshManager->loadMeshPrimitive(info, shaderID);
}

void fillCubeMeshData(MeshData& info, const std::string& name, const Vec3& size) {
	constexpr int vertexCount = 36;

	info.path = name;
	info.numVertices = vertexCount;
	info.numIndices = vertexCount;
	info.numTriangles = 12;

	info.vertices = new Vertex[vertexCount];
	info.indices = new unsigned int[vertexCount];

	float x = 0.5f * size.x;
	float y = 0.5f * size.y;
	float z = 0.5f * size.z;

	Vec3 positions[8] = {
		{-x, -y, -z}, { x, -y, -z}, { x,  y, -z}, { -x,  y, -z}, // Back
		{-x, -y,  z}, { x, -y,  z}, { x,  y,  z}, { -x,  y,  z}  // Front
	};

	unsigned int faces[6][6] = {
		{4, 5, 6, 4, 6, 7}, // Front
		{1, 0, 3, 1, 3, 2}, // Back
		{0, 4, 7, 0, 7, 3}, // Left
		{5, 1, 2, 5, 2, 6}, // Right
		{3, 7, 6, 3, 6, 2}, // Top
		{0, 1, 5, 0, 5, 4}  // Bottom
	};

	for (int i = 0; i < 6; ++i) {
		for (int j = 0; j < 6; ++j) {
			int idx = i * 6 + j;

			info.vertices[idx].pos = { positions[faces[i][j]], 0.0f };
			info.vertices[idx].col = { 1.0f, 1.0f, 1.0f, 1.0f };
			info.indices[idx] = idx;
		}
	}

	return;
}

bool createCube(MeshManager* meshManager, const std::string& name, const Vec3& size) {
	MeshData info;
	info.path = name;

	fillCubeMeshData(info, name, size);

	if (!meshManager->loadMeshPrimitive(info, 0)) {
		fprintf(stderr, "createCube: LoadPrimitiveIntoVAO failed for %s\n", name.c_str());
		return false;
	}

	return true;
}