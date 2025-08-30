#pragma once

#include "objects/mesh.hpp"
#include "loaders/meshLoader.hpp"
#include <string>
#include <map>

class MeshManager {
public:
	MeshManager() = default;
	~MeshManager();

	bool addMesh(const std::string& path);
	bool addMesh(const std::string& path, Mesh& mesh);

	bool findMesh(const std::string& path) const;
	bool getMesh(const std::string& path, Mesh*& dataOut);

private:
	MeshLoader loader;
	std::map<std::string, Mesh> pathToMeshes;
};