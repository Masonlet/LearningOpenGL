#include "factory.hpp"

#include <cmath>

static bool fillCubeMeshData(ModelDrawInfo& info, const std::string& name, const Vec3& size) {
	info.meshName = name;
	info.numVertices = 36;
	info.numIndices = 36;
	info.numTriangles = 12;

	info.vertices = new Vertex[36];
	info.indices = new unsigned int[36];

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
			info.vertices[idx].pos = positions[faces[i][j]];
			info.vertices[idx].col = {1.0f, 1.0f, 1.0f, 1.0f};
			info.indices[idx] = idx;
		}
	}

	return true;
}

bool createTriangle(VAOManager* vaoManager, const std::string& name, const Vec2& size, const Vec4& vertexColour) {
	ModelDrawInfo info;
	info.meshName = name;
	info.numVertices = 3;
	info.numIndices = 3;
	info.numTriangles = 1;
	
	info.vertices = new Vertex[3];
	info.indices = new unsigned int[3];
	
	info.vertices[0].pos = { -0.5f * size.x, -0.5f * size.y, 0.0f };
	info.vertices[1].pos = {  0.5f * size.x, -0.5f * size.y, 0.0f };
	info.vertices[2].pos = {  0.0f,           0.5f * size.y, 0.0f };

	for (int i = 0; i < 3; ++i)
		info.vertices[i].col = vertexColour;

	info.indices[0] = 0;
	info.indices[1] = 1;
	info.indices[2] = 2;

	return vaoManager->LoadPrimitiveIntoVAO(info, 0);
}
bool createSquare(VAOManager* vaoManager, const std::string& name, const Vec2& size) {
	ModelDrawInfo info;
	info.meshName = name;
	info.numVertices = 6;
	info.numIndices = 6;
	info.numTriangles = 2;

	info.vertices = new Vertex[6];
	info.indices = new unsigned int[6];

	float halfX = 0.5f * size.x;
	float halfY = 0.5f * size.y;

	// First triangle
	info.vertices[0].pos = { -halfX, -halfY, 0.0f };
	info.vertices[1].pos = {  halfX, -halfY, 0.0f };
	info.vertices[2].pos = {  halfX,  halfY, 0.0f };

	// Second triangle
	info.vertices[3].pos = { -halfX, -halfY, 0.0f };
	info.vertices[4].pos = {  halfX,  halfY, 0.0f };
	info.vertices[5].pos = { -halfX,  halfY, 0.0f };

	for (int i = 0; i < 6; ++i)
		info.vertices[i].col = { 1.0f, 1.0f, 1.0f, 1.0f };

	for (int i = 0; i < 6; ++i)
		info.indices[i] = i;

	return vaoManager->LoadPrimitiveIntoVAO(info, 0);
}
bool createCube(VAOManager* vaoManager, const std::string& name, const Vec3& size) {
	ModelDrawInfo info;
	info.meshName = name;
	if (!fillCubeMeshData(info, name, size)) return false;

	if (!vaoManager->LoadPrimitiveIntoVAO(info, 0)) {
		fprintf(stderr, "createCube: LoadPrimitiveIntoVAO failed for %s\n", name.c_str());
		return false;
	}

	return true;
}

bool createSquareGrid(Engine& engine, const std::string& baseName, int startIndex, int count, const Vec2& spacing, const Vec3& rotation, const Vec2& size) {
	int gridSize = static_cast<int>(std::ceil(std::sqrt(count)));

	std::string sharedName = baseName + "_sharedSquare";

  ModelDrawInfo drawInfo;
  if (!engine.getMeshManager()->FindDrawInfoByModelName(sharedName, drawInfo)) {
    if (!createSquare(engine.getMeshManager(), sharedName, size))
      return false;

    if (!engine.getMeshManager()->FindDrawInfoByModelName(sharedName, drawInfo))
      return false;

    engine.addModelInfo(sharedName, drawInfo); 
  }

	for (int i = startIndex; i < count; ++i) {
		int row = i / gridSize;
		int col = i % gridSize;

		Vec3 position = {
			spacing.x * col,
			spacing.y * row,
			0.0f
		};

		std::string instanceName = baseName + "_instance_" + std::to_string(i);

		Mat4 transform = Mat4::translation(position);
		engine.addInstance(instanceName, sharedName, transform);
	}

	return true;
}
bool createCubeGrid(Engine& engine, const std::string& baseName, int startIndex, int count, const Vec2& spacing, const Vec3& rotation, const Vec3& size) {
	int gridSize = static_cast<int>(std::ceil(std::sqrt(count)));

	std::string sharedName = baseName + "_sharedCube";

	ModelDrawInfo info;
	if (!engine.getMeshManager()->FindDrawInfoByModelName(sharedName, info)) {
		info.meshName = sharedName;
		if (!fillCubeMeshData(info, sharedName, size)) return false;

		// Upload to GPU through VAOManager
		if (!engine.getMeshManager()->LoadPrimitiveIntoVAO(info, engine.getProgram())) {
			fprintf(stderr, "createCubeGrid: LoadPrimitiveIntoVAO failed for %s\n", sharedName.c_str());
			return false;
		}

		engine.addModelInfo(sharedName, info);
	}
	
	for (int i = startIndex; i < count; ++i) {
		int row = i / gridSize;
		int col = i % gridSize;

		Vec3 position = {
			spacing.x * col,
			0.0f,
			spacing.y * row,
		};

		std::string instanceName = baseName + "_instance_" + std::to_string(i);

		Mat4 transform = Mat4::translation(position);
		if (!engine.addInstance(instanceName, sharedName, transform)) {
			fprintf(stderr, "Failed to add instance: %s\n", instanceName.c_str());
			return false;
		}
	}

	return true;
}

bool createMeshFromPath(VAOManager& vaoManager, const std::string& name, const std::string& path, const Vec3& scale, bool hasNormals) {
	ModelDrawInfo info;
	info.meshName = name;

	if (!vaoManager.LoadModelFromFile(path, info))
		return false;

	for (unsigned int i = 0; i < info.numVertices; ++i) {
		info.vertices[i].pos.x *= scale.x;
		info.vertices[i].pos.y *= scale.y;
		info.vertices[i].pos.z *= scale.z;
	}

	return vaoManager.LoadModelIntoVAO(name, info, 0);
}
bool createMeshGridFromPath(VAOManager& vaoManager, const std::string& baseName, const std::string& path, int startIndex, int count, const Vec2& spacing, const Vec3& rotation, const Vec3& scale, bool hasNormals) {
	int gridSize = static_cast<int>(std::ceil(std::sqrt(count)));

	std::string sharedName = baseName + "_sharedMesh";

	ModelDrawInfo info;
	if (!vaoManager.FindDrawInfoByModelName(sharedName, info)) {
		if (!createMeshFromPath(vaoManager, sharedName, path, scale, hasNormals))
			return false;

		if (!vaoManager.FindDrawInfoByModelName(sharedName, info)) {
			 fprintf(stderr, "createCubeGrid: LoadPrimitiveIntoVAO failed for %s\n", sharedName.c_str());

			delete[] info.vertices;
			delete[] info.indices;
			return false;
		}
	}

	for (int i = 0; i < count; ++i) {
		int row = i / gridSize;
		int col = i % gridSize;

		Vec3 position = {
			spacing.x * col,
			spacing.y * row,
			0.0f
		};

		std::string instanceName = baseName + "_instance_" + std::to_string(i);

		ModelDrawInfo instance = info;
		instance.meshName = instanceName;
		vaoManager.LoadModelIntoVAO(instanceName, instance, 0);
	}

	return true;
}
