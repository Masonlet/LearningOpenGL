#include <glad/glad.h> 

#include "models/grids.hpp"
#include "models/primitives.hpp"

#include <cmath>

static Vec3 calculateGridPosition(const int index, const int gridSize, const Vec2& spacing, bool zUp = false) {
	int row = index / gridSize;
	int col = index % gridSize;

	return zUp ? Vec3{ spacing.x * col, 0.0f, spacing.y * row }
				     : Vec3{ spacing.x * col, spacing.y * row, 0.0f };
}

bool createSquareGrid(MeshManager* meshManager, unsigned int shaderProgramID, const std::string& baseName, int startIndex, int count, const Vec2& spacing, const Vec3& rotation, const Vec2& size) {
	int gridSize = static_cast<int>(std::ceil(std::sqrt(count)));
	std::string sharedName = baseName + "_sharedSquare";

	const MeshData* drawInfo{};
	if (!meshManager->findMesh(sharedName, drawInfo)) {
		MeshData temp;
		if (!createSquare(meshManager, sharedName, shaderProgramID, size))
			return false;

		if (!meshManager->findMesh(sharedName, drawInfo)) {
			fprintf(stderr, "createSquareGrid: mesh not found after creation\n");
			return false;
		}
	}

	return true;
}

bool createCubeGrid(MeshManager* meshManager, unsigned int shaderProgramID, const std::string& baseName, int startIndex, int count, const Vec2& spacing, const Vec3& rotation, const Vec3& size) {
	int gridSize = static_cast<int>(std::ceil(std::sqrt(count)));

	const std::string sharedName = baseName + "_sharedCube";

  const MeshData* info = nullptr; 
	if (!meshManager->findMesh(sharedName, info)) {
		MeshData temp;
		temp.path = sharedName;

		fillCubeMeshData(temp, sharedName, size);

		if (!meshManager->loadMeshPrimitive(temp, shaderProgramID)) {
			fprintf(stderr, "createCubeGrid: LoadPrimitiveIntoVAO failed for %s\n", sharedName.c_str());
			return false;
		}

		meshManager->findMesh(sharedName, info);
	}

	return true;
}

bool createMeshGridFromPath(MeshManager* meshManager, unsigned int shaderProgramID, const std::string& baseName, const std::string& path, int startIndex, int count, const Vec2& spacing, const Vec3& rotation, const Vec3& scale, bool hasNormals) {
	int gridSize = static_cast<int>(std::ceil(std::sqrt(count)));
	std::string sharedName = baseName + "_sharedMesh";

	const MeshData* info = nullptr;
	if (!meshManager->findMesh(sharedName, info)) {
		MeshData tmp;
		if (!meshManager->loadMeshFile(path, tmp, shaderProgramID) ||
			!meshManager->findMesh(path, info)) {
			fprintf(stderr, "[createMeshFromPath] loadMesh failed for %s\n", path.c_str());
			return false;
		}
	}

	return true;
}