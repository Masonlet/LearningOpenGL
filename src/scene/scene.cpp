#include <glad/glad.h>

#include "scene/scene.hpp"

Scene::~Scene() {
  modelInfos.clear();
  modelInstances.clear();
}

void Scene::setSceneName(const std::string& sceneNameIn) {
  sceneName = sceneNameIn;
}

void Scene::addModelInfo(const std::string& name, const ModelDrawInfo* info) {
  modelInfos[name] = info;
}

bool Scene::addInstance(const std::string& name, const std::string& path, const Mat4& transform) {
  if (modelInstances.find(name) != modelInstances.end()) {
    printf("[warn] instance name already used: %s\n", name.c_str());
    return false;
  }

  if (modelInfos.find(path) == modelInfos.end()) {
    printf("[error] cannot add instance: mesh not preloaded: %s\n", path.c_str());
    return false;
  }

  Transform t = transform.decompose();

  ModelInstance instance;
  instance.position = t.position;
  instance.rotation = t.rotation;
  instance.scale = t.scale;
  instance.modelMatrix = transform;
  instance.path = path;

  modelInstances[name] = instance;
  return true;
}