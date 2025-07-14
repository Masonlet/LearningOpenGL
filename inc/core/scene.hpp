#pragma once

#include "core/modelInstance.hpp"
#include "graphics/vaoManager.hpp"
#include <map>
#include <string>

class Scene {
public:
  std::string& getSceneName() { return sceneName; }
  std::map<std::string, ModelInstance>& getModelInstances() { return modelInstances; }
  std::map<std::string, ModelDrawInfo>& getModelInfos() { return modelInfos; }

  void setSceneName(const std::string& sceneNameIn);
  void addModelInfo(const std::string& name, const ModelDrawInfo& info);
  bool addInstance(const std::string& name, const std::string& path, const Mat4& transform);
  void clearModels(VAOManager& vaoManager);

private:
  std::string sceneName;
  std::map<std::string, ModelInstance> modelInstances;
  std::map<std::string, ModelDrawInfo> modelInfos;
};