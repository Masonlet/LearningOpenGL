#pragma once

#include "graphics/mesh.hpp"
#include <string>
#include <map>

class MeshManager {
public:
	MeshManager() = default;
	~MeshManager();

	bool uploadMeshToGPU(const std::string& path, Mesh& mesh, unsigned int shaderID);
	
	bool findMesh(const std::string& name) const;
	bool getMesh(const std::string& name, Mesh*& dataOut);

private:
	std::map<std::string, Mesh> nameToMeshes;
};