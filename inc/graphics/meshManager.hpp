#pragma once

#include "objects/meshData.hpp"
#include <string>
#include <map>

class MeshManager {
public:
	MeshManager() = default;
	~MeshManager();

	bool loadMeshFile(const std::string& name, unsigned int shaderID);
	bool loadMeshPrimitive(MeshData& mesh, unsigned int shaderID);
	bool findMesh(const std::string& name) const;
	bool getMesh(const std::string& name, MeshData*& data);

private:
	std::map<std::string, MeshData> nameToMeshes;
	bool UploadMeshToGPU(MeshData& mesh, unsigned int shaderID);
};