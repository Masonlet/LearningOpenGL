#include "generator.hpp"
#include <cmath>
#include <cstdio>

bool createTriangle(Mesh& mesh, const Vec2& size) {
	unsigned int vertexCount{3}, triangleCount{1};

	mesh.numVerticesToDraw = vertexCount;
	mesh.numIndicesToDraw = triangleCount * 3;

	if (!mesh.allocate(vertexCount, triangleCount)) 
		return {};
	
	mesh.vertices[0] = { {-0.5f * size.x, -0.5f * size.y, 0.0f}, DEFAULT_NORMAL, DEFAULT_COLOUR };
	mesh.vertices[1] = { {0.5f * size.x, -0.5f * size.y, 0.0f}, DEFAULT_NORMAL, DEFAULT_COLOUR };
	mesh.vertices[2] = { {0.0f * size.x, 0.5f * size.y, 0.0f}, DEFAULT_NORMAL, DEFAULT_COLOUR };

	mesh.indices[0] = 0;
	mesh.indices[1] = 1;
	mesh.indices[2] = 2;

	mesh.upload();

	return true;
}

bool createSquare(Mesh& mesh, const Vec2& size) {
	unsigned int vertexCount{4}, triangleCount{2};

	if (!mesh.allocate(vertexCount, triangleCount))
		return {};

	mesh.numVerticesToDraw = vertexCount;
	mesh.numIndicesToDraw = triangleCount * 3;

	mesh.vertices[0] = { {-0.5f * size.x, -0.5f * size.y, 0.0f}, DEFAULT_NORMAL, DEFAULT_COLOUR }; 
	mesh.vertices[1] = { { 0.5f * size.x, -0.5f * size.y, 0.0f}, DEFAULT_NORMAL, DEFAULT_COLOUR }; 
	mesh.vertices[2] = { { 0.5f * size.x,  0.5f * size.y, 0.0f}, DEFAULT_NORMAL, DEFAULT_COLOUR }; 
	mesh.vertices[3] = { {-0.5f * size.x,  0.5f * size.y, 0.0f}, DEFAULT_NORMAL, DEFAULT_COLOUR };

	mesh.indices[0] = 0;
	mesh.indices[1] = 1; 
	mesh.indices[2] = 2; 
	mesh.indices[3] = 2; 
	mesh.indices[4] = 3; 
	mesh.indices[5] = 0; 

	mesh.upload();

	return true;
}
bool createSquareGrid(Model* models, int startIndex, int count, const Vec2& spacing, const Vec3& rotation, const Vec2& size) {
	int gridSize = static_cast<int>(std::ceil(std::sqrt(count)));

	Mesh* squareMesh = new Mesh();
	if (!createSquare(*squareMesh, size)) 
		return false;

	squareMesh->upload();

	if (squareMesh == nullptr || squareMesh->vao == 0) 
		return false;

	for (size_t i = 0; i < count; i++) {
		size_t row = i / gridSize; 
		size_t col = i % gridSize;

		models[i].mesh = squareMesh;
		models[i].transform.position = { spacing.x * col, spacing.y * row , 0.0f};
		models[i].transform.rotation = rotation;
	}

	return true;
}

bool createCube(Mesh& mesh, const Vec3& size) {
	unsigned int vertexCount{24}, triangleCount{12};

	if (!mesh.allocate(vertexCount, triangleCount))
		return false;

	mesh.numVerticesToDraw = vertexCount;
	mesh.numIndicesToDraw = triangleCount * 3;

	const float cubeWidth = size.x * 0.5f;
	const float cubeHeight = size.y * 0.5f;
	const float cubeDepth = size.z * 0.5f;
	mesh.vertices[0] = {{-cubeWidth, -cubeHeight, cubeDepth}, DEFAULT_NORMAL, DEFAULT_COLOUR}; 
	mesh.vertices[1] = {{ cubeWidth, -cubeHeight, cubeDepth}, DEFAULT_NORMAL, DEFAULT_COLOUR}; 
	mesh.vertices[2] = {{ cubeWidth,  cubeHeight, cubeDepth}, DEFAULT_NORMAL, DEFAULT_COLOUR};
	mesh.vertices[3] = {{-cubeWidth,  cubeHeight, cubeDepth}, DEFAULT_NORMAL, DEFAULT_COLOUR};

	mesh.vertices[4] = {{ cubeWidth, -cubeHeight,  -cubeDepth}, DEFAULT_NORMAL, DEFAULT_COLOUR};
	mesh.vertices[5] = {{-cubeWidth, -cubeHeight,  -cubeDepth}, DEFAULT_NORMAL, DEFAULT_COLOUR}; 
	mesh.vertices[6] = {{-cubeWidth,  cubeHeight,  -cubeDepth}, DEFAULT_NORMAL, DEFAULT_COLOUR};
	mesh.vertices[7] = {{ cubeWidth,  cubeHeight,  -cubeDepth}, DEFAULT_NORMAL, DEFAULT_COLOUR}; 

	mesh.vertices[8]  = {{-cubeWidth, -cubeHeight, -cubeDepth}, DEFAULT_NORMAL, DEFAULT_COLOUR}; 
	mesh.vertices[9]  = {{-cubeWidth, -cubeHeight,  cubeDepth}, DEFAULT_NORMAL, DEFAULT_COLOUR}; 
	mesh.vertices[10] = {{-cubeWidth,  cubeHeight,  cubeDepth}, DEFAULT_NORMAL, DEFAULT_COLOUR}; 
	mesh.vertices[11] = {{-cubeWidth,  cubeHeight, -cubeDepth}, DEFAULT_NORMAL, DEFAULT_COLOUR}; 

	mesh.vertices[12] = {{ cubeWidth, -cubeHeight,  cubeDepth}, DEFAULT_NORMAL, DEFAULT_COLOUR}; 
	mesh.vertices[13] = {{ cubeWidth, -cubeHeight, -cubeDepth}, DEFAULT_NORMAL, DEFAULT_COLOUR}; 
	mesh.vertices[14] = {{ cubeWidth,  cubeHeight, -cubeDepth}, DEFAULT_NORMAL, DEFAULT_COLOUR}; 
	mesh.vertices[15] = {{ cubeWidth,  cubeHeight,  cubeDepth}, DEFAULT_NORMAL, DEFAULT_COLOUR}; 

	mesh.vertices[16] = {{-cubeWidth,  cubeHeight,  cubeDepth}, DEFAULT_NORMAL, DEFAULT_COLOUR}; 
	mesh.vertices[17] = {{ cubeWidth,  cubeHeight,  cubeDepth}, DEFAULT_NORMAL, DEFAULT_COLOUR}; 
	mesh.vertices[18] = {{ cubeWidth,  cubeHeight, -cubeDepth}, DEFAULT_NORMAL, DEFAULT_COLOUR}; 
	mesh.vertices[19] = {{-cubeWidth,  cubeHeight, -cubeDepth}, DEFAULT_NORMAL, DEFAULT_COLOUR}; 

	mesh.vertices[20] = {{-cubeWidth, -cubeHeight, -cubeDepth}, DEFAULT_NORMAL, DEFAULT_COLOUR}; 
	mesh.vertices[21] = {{ cubeWidth, -cubeHeight, -cubeDepth}, DEFAULT_NORMAL, DEFAULT_COLOUR}; 
	mesh.vertices[22] = {{ cubeWidth, -cubeHeight,  cubeDepth}, DEFAULT_NORMAL, DEFAULT_COLOUR}; 
	mesh.vertices[23] = {{-cubeWidth, -cubeHeight,  cubeDepth}, DEFAULT_NORMAL, DEFAULT_COLOUR}; 

	mesh.indices[0]  = 0;
	mesh.indices[1]  = 1;
	mesh.indices[2]  = 2;
	mesh.indices[3]  = 2;
	mesh.indices[4]  = 3;
	mesh.indices[5]  = 0;

	mesh.indices[6]  = 4;
	mesh.indices[7]  = 5;
	mesh.indices[8]  = 6;
	mesh.indices[9]  = 6;
	mesh.indices[10] = 7;
	mesh.indices[11] = 4;

	mesh.indices[12] = 8;
	mesh.indices[13] = 9;
	mesh.indices[14] = 10;
	mesh.indices[15] = 10;
	mesh.indices[16] = 9;
	mesh.indices[17] = 8;

	mesh.indices[18] = 12;
	mesh.indices[19] = 13;
	mesh.indices[20] = 14;
	mesh.indices[21] = 14;
	mesh.indices[22] = 15;
	mesh.indices[23] = 12;

	mesh.indices[24] = 16;
	mesh.indices[25] = 17;
	mesh.indices[26] = 18;
	mesh.indices[27] = 18;
	mesh.indices[28] = 19;
	mesh.indices[29] = 16;

	mesh.indices[30] = 20;
	mesh.indices[31] = 21;
	mesh.indices[32] = 22;
	mesh.indices[33] = 22;
	mesh.indices[34] = 23;
	mesh.indices[35] = 20;

	mesh.upload();
	return true;
}
bool createCubeGrid(Model* models, int startIndex, int count, const Vec2& spacing, const Vec3& rotation, const Vec3& size) {
	int gridSize = static_cast<int>(std::ceil(std::sqrt(count)));

	Mesh* cubeMesh = new Mesh();
	if (!createCube(*cubeMesh, size))
		return false;

	cubeMesh->upload();
	for (size_t i = 0; i < count; i++) {
		size_t row = i / gridSize; 
		size_t col = i % gridSize;

		models[i].mesh = cubeMesh;
		models[i].transform.position = { spacing.x * col, 0.0f, spacing.y * row };
		models[i].transform.rotation = rotation; 
	}

	return true;
}

bool createMeshPath(Mesh* mesh, const char* path, const char* type, const Vec3& scale, const bool hasNormals) {
	if (type[0] == 'P' && type[1] == 'L' && type[2] == 'Y')
		return mesh->createPLY(path, hasNormals);

	return false;
}
bool createModelGrid(Model* models, const char* path, int startIndex, int count, const Vec2& spacing, const Vec3& rotation, const Vec3& scale, const bool hasNormals) {
	int gridSize = static_cast<int>(std::ceil(std::sqrt(count)));
	
	Mesh* modelMesh = new Mesh();
	if (!createMeshPath(modelMesh, path, "PLY", scale, hasNormals)) {
		fprintf(stderr, "Failed to load base model from path: %s\n", path);
		return false;
	}

	modelMesh->upload();
	for (size_t i = 0; i < count; ++i) {
		size_t row = i / gridSize;
		size_t col = i % gridSize;

		models[i].mesh = modelMesh;
		models[i].transform.scale(scale);
		models[i].transform.position = { spacing.x * col, 0.0f, spacing.y * row };
		models[i].transform.rotation = rotation;
	}

	return true;
}