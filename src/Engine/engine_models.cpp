#include "engine.hpp"

void Engine::addModelInfo(const std::string& name, const ModelDrawInfo& info) {
  modelInfos[name] = info;
}

bool Engine::addInstance(const std::string& name, const std::string& path, const Mat4& transform) {
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

bool Engine::loadModel(const std::string& name, const std::string& path,const Vec3& position, const Vec3& rotation, const Vec3& scale,const Vec3& assignedColour, ColourMode colourMode) {
  if (modelInstances.find(name) != modelInstances.end()) {
    printf("name already used: %s\n", path.c_str());
    return false;
  }

  if (modelInfos.find(path) == modelInfos.end()) {
    ModelDrawInfo meshinfo;
    meshinfo.meshName = path;
    meshinfo.colour = assignedColour; 
    meshinfo.colourMode = colourMode;

    if (!meshManager->LoadModelIntoVAO(path, meshinfo, program)) {
      printf("failed to load model into vao: %s\n", path.c_str());
      return false;
    }	

    modelInfos[path] = meshinfo;
  }

 Transform t;
  t.position = position;
  t.rotation = rotation;
  t.scale = scale;

  ModelInstance instance;
  instance.position = position;
  instance.rotation = rotation;
  instance.scale = scale;
  instance.modelMatrix = Mat4::modelMatrix(t);
  instance.colour = assignedColour;
  instance.colourMode = colourMode;
  instance.path = path;

  modelInstances[name] = instance;
  return true;
}	

void Engine::unloadModel(ModelDrawInfo& info) {
  glDeleteVertexArrays(1, &info.VAO_ID);
  glDeleteBuffers(1, &info.VertexBufferID);
  glDeleteBuffers(1, &info.IndexBufferID);

  delete[] info.vertices;
  delete[] info.indices;
}

void Engine::clearModels() {
  for (std::map<std::string, ModelDrawInfo>::iterator it = modelInfos.begin(); it != modelInfos.end(); ++it) {
    ModelDrawInfo& info = it->second;
    unloadModel(info);
  }
  modelInfos.clear();
  modelInstances.clear();
}

void Engine::incrementModel(){
  if (modelInstances.empty()) return;
  currentModel = (currentModel + 1) % modelInstances.size();
}

void Engine::decrementModel(){
  if (modelInstances.empty()) return;
  currentModel = (currentModel + modelInstances.size() - 1) % modelInstances.size();
}
