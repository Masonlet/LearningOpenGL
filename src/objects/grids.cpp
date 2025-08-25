#include <glad/glad.h> 
#include "objects/grids.hpp"
#include "objects/primitives.hpp"
#include "utils/log.hpp"
#include <cmath>

static Vec3 calculateGridPosition(const int index, const int gridSize, const Vec2& spacing, bool zUp = false) {
	int row = index / gridSize;
	int col = index % gridSize;

	return zUp ? Vec3{ spacing.x * col, 0.0f, spacing.y * row }
				     : Vec3{ spacing.x * col, spacing.y * row, 0.0f };
}

bool createSquareGrid(MeshManager* meshManager, unsigned int shaderProgramID, const std::string& baseName, int startIndex, int count, const Vec2& spacing, const Vec3& rot, const Vec2& size) {
	int gridSize = static_cast<int>(std::ceil(std::sqrt(count)));
	std::string sharedName = baseName + "_sharedSquare";

	if (!meshManager->findMesh(sharedName)) 
		if (!createSquare(meshManager, sharedName, shaderProgramID, size)) 
			return error("Grids", "createSquareGrid", "Failed to create square mesh");

	return debugLog("Grids", "createSquareGrid", "Created with " + std::to_string(count) + " squares.");
}

bool createCubeGrid(MeshManager* meshManager, unsigned int shaderProgramID, const std::string& baseName, int startIndex, int count, const Vec2& spacing, const Vec3& rot, const Vec3& size) {
	int gridSize = static_cast<int>(std::ceil(std::sqrt(count)));
	const std::string sharedName = baseName + "_sharedCube";

	if (!meshManager->findMesh(sharedName)) 
		if (!createCube(meshManager, sharedName, size)) 
			return error("Grids", "createCubeGrid", "Failed to create cube mesh");
	
	return debugLog("Grids", "createCubeGrid", "Created with " + std::to_string(count) + " cubes.");
}

bool createMeshGridFromPath(MeshManager* meshManager, unsigned int shaderProgramID, const std::string& baseName, const std::string& path, int startIndex, int count, const Vec2& spacing, const Vec3& rot, const Vec3& size, bool hasNormals) {
	int gridSize = static_cast<int>(std::ceil(std::sqrt(count)));
	std::string sharedName = baseName + "_sharedMesh";

	if (!meshManager->findMesh(sharedName)) 
		if (!meshManager->UploadPathToGPU(path, shaderProgramID)) 
			return error("Grids", "createMeshGrid", "Failed to load mesh");

	return true;
}