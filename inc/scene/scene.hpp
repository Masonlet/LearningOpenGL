#pragma once

#include "models/modelData.hpp"
#include <map>
#include <string>

class Scene {
public:
  Scene() {};
  ~Scene();

  std::string& getSceneName() { return name; }
  std::map<std::string, ModelData>& getModelData() { return data; }

  inline void setSceneName(const std::string& sceneName) { name = sceneName; }
  bool addInstance(const ModelData& modelInstance);

private:
  std::string name;
  std::map<std::string, ModelData> data;
};