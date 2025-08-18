#pragma once

#include "graphics/meshData.hpp"
#include <string>
#include <map>

class MeshManager {
public:
	bool loadMeshFile(const std::string& name, MeshData& mesh, unsigned int shaderID);
	bool loadMeshPrimitive(MeshData& mesh, unsigned int shaderID);
	bool findMesh(const std::string& name, const MeshData*& mesh) const;
	void Shutdown();

private:
	std::map<std::string, MeshData> nameToMeshes;
	bool UploadMeshToGPU(MeshData& mesh, unsigned int shaderID);
};