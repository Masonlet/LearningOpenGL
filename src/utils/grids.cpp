#include "utils/grids.hpp"
#include "utils/primitives.hpp"
#include "core/meshLoader.hpp"

#include <cmath>

static Vec3 calculateGridPosition(const int index, const int gridSize, const Vec2& spacing, bool zUp = false) {
	int row = index / gridSize;
	int col = index % gridSize;

	if (zUp)
		return Vec3{ spacing.x * col, 0.0f, spacing.y * row };
	else
		return Vec3{ spacing.x * col, spacing.y * row, 0.0f };
}

bool createSquareGrid(SceneLoader& loader, const std::string& baseName, int startIndex, int count, const Vec2& spacing, const Vec3& rotation, const Vec2& size) {
	int gridSize = static_cast<int>(std::ceil(std::sqrt(count)));

	std::string sharedName = baseName + "_sharedSquare";

	ModelDrawInfo drawInfo;
	if (!loader.getRenderer()->getVAOManager()->FindDrawInfoByModelName(sharedName, drawInfo)) {
		if (!createSquare(loader.getRenderer()->getVAOManager(), sharedName, loader.getRenderer()->getProgram(), size))
			return false;

		if (!loader.getRenderer()->getVAOManager()->FindDrawInfoByModelName(sharedName, drawInfo)) {
			fprintf(stderr, "createSquareGrid: mesh not found after creation\n");
			return false;
		}

		loader.getScene().addModelInfo(sharedName, drawInfo);
	}

	for (int i = startIndex; i < count; ++i) {
		Vec4 position = { calculateGridPosition(i, gridSize, spacing), 0.0f };

		std::string instanceName = baseName + "_instance_" + std::to_string(i);

		Mat4 transform = Mat4::translation(position);
		loader.getScene().addInstance(instanceName, sharedName, transform);
	}

	return true;
}

bool createCubeGrid(SceneLoader& loader, const std::string& baseName, int startIndex, int count, const Vec2& spacing, const Vec3& rotation, const Vec3& size) {
	int gridSize = static_cast<int>(std::ceil(std::sqrt(count)));

	std::string sharedName = baseName + "_sharedCube";

	ModelDrawInfo info;
	if (!loader.getRenderer()->getVAOManager()->FindDrawInfoByModelName(sharedName, info)) {
		info.meshPath = sharedName;
		if (!fillCubeMeshData(info, sharedName, size)) return false;

		// Upload to GPU through VAOManager
		if (!loader.getRenderer()->getVAOManager()->LoadPrimitiveIntoVAO(info, loader.getRenderer()->getProgram())) {
			fprintf(stderr, "createCubeGrid: LoadPrimitiveIntoVAO failed for %s\n", sharedName.c_str());
			return false;
		}

		loader.getScene().addModelInfo(sharedName, info);
	}

	for (int i = startIndex; i < count; ++i) {
		Vec4 position = { calculateGridPosition(i, gridSize, spacing, true), 0.0f };

		std::string instanceName = baseName + "_instance_" + std::to_string(i);

		Mat4 transform = Mat4::translation(position);
		if (!loader.getScene().addInstance(instanceName, sharedName, transform)) {
			fprintf(stderr, "Failed to add instance: %s\n", instanceName.c_str());
			return false;
		}
	}

	return true;
}

bool createMeshGridFromPath(SceneLoader& loader, const std::string& baseName, const std::string& path, int startIndex, int count, const Vec2& spacing, const Vec3& rotation, const Vec3& scale, bool hasNormals) {
	int gridSize = static_cast<int>(std::ceil(std::sqrt(count)));
	std::string sharedName = baseName + "_sharedMesh";

	ModelDrawInfo info;
	if (!loader.getRenderer()->getVAOManager()->FindDrawInfoByModelName(sharedName, info)) {
		if (!createMeshFromPath(*loader.getRenderer()->getVAOManager(), sharedName, path, scale, hasNormals))
			return false;

		loader.getScene().addModelInfo(sharedName, info);
	}

	for (int i = 0; i < count; ++i) {
		Vec4 position = { calculateGridPosition(i, gridSize, spacing), 0.0f };
		std::string instanceName = baseName + "_instance_" + std::to_string(i);
		Mat4 transform = Mat4::translation(position);

		if (!loader.getScene().addInstance(instanceName, sharedName, transform)) {
			fprintf(stderr, "Failed to add instance: %s\n", instanceName.c_str());
			return false;
		}
	}

	return true;
}