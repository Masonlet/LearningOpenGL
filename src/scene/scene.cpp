#include <glad/glad.h>

#include "scene/scene.hpp"
#include "core/engine.hpp"
#include "utils/files.hpp"
#include "utils/parser.hpp"
#include "utils/primitives.hpp"

#include <cstring>
#include <fstream>
#include <iomanip>

void Scene::setSceneName(const std::string& sceneNameIn) {
  sceneName = sceneNameIn;
}

void Scene::addModelInfo(const std::string& name, const ModelDrawInfo& info) {
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

void Scene::clearModels(VAOManager& vaoManager) {
  std::map<std::string, ModelDrawInfo>::iterator it = modelInfos.begin();
  for (it; it != modelInfos.end(); ++it) {
    ModelDrawInfo& info = it->second;

    glDeleteVertexArrays(1, &info.VAO_ID);
    glDeleteBuffers(1, &info.VertexBufferID);
    glDeleteBuffers(1, &info.IndexBufferID);

    if (info.vertices) {
      delete[] info.vertices;
      info.vertices = nullptr;
    }
    if (info.indices) {
      delete[] info.indices;
      info.indices = nullptr;
    }
  }

  modelInfos.clear();
  modelInstances.clear();
}