#pragma once

#include "core/modelInstance.hpp"
#include "graphics/vaoManager.hpp"

#include <map>
#include <string>

class Scene {
public:
  Scene() {};
  ~Scene();

  std::string& getSceneName() { return sceneName; }
  std::map<std::string, const ModelDrawInfo*>& getModelInfos() { return modelInfos; }
  std::map<std::string, ModelInstance>& getModelInstances() { return modelInstances; }

  void setSceneName(const std::string& sceneNameIn);
  void addModelInfo(const std::string& name, const ModelDrawInfo* info);
  bool addInstance(const std::string& name, const std::string& path, const Mat4& transform);

  void clearModels(VAOManager& vaoManager);

private:
  std::string sceneName;
  std::map<std::string, const ModelDrawInfo*> modelInfos;
  std::map<std::string, ModelInstance> modelInstances;
};