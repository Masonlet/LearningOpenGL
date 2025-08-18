#include "scene/scene.hpp"

Scene::~Scene() {
  data.clear();
}

bool Scene::addInstance(const ModelData& modelData) {
  if (modelData.name.empty()) {
    printf("[warn] instance name is empty\n");
    return false;
	}

	const std::string name = modelData.name;

  if (data.find(name) != data.end()) {
    printf("[warn] instance name already used: %s\n", name.c_str());
    return false;
  }

	data.emplace(name, modelData);
  return true;
}