#include "core/meshLoader.hpp"

bool createMeshFromPath(VAOManager& vaoManager, const std::string& name, const std::string& path, const Vec3& scale, bool hasNormals) {
	ModelDrawInfo info;
	info.meshPath = name;

	if (!vaoManager.LoadModelFromFile(path, info))
		return false;

	for (unsigned int i = 0; i < info.numVertices; ++i) {
		info.vertices[i].pos.x *= scale.x;
		info.vertices[i].pos.y *= scale.y;
		info.vertices[i].pos.z *= scale.z;
	}

	return vaoManager.LoadModelIntoVAO(name, info, 0);
}