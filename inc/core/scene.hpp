#pragma once

#include "utils/log.hpp"
#include "objects/model.hpp"
#include "objects/light.hpp"
#include "objects/camera.hpp"
#include "objects/grid.hpp"
#include "objects/textureData.hpp"
#include <map>
#include <string>

class Scene {
public:
  inline void setSceneName(const std::string& sceneName) { name = sceneName; }
  std::string& getSceneName() { return name; }

  template <typename T, typename MapT>
  bool addObject(MapT& map, const T& data, const char* type) {
    if (data.name.empty()) return error("Scene", "addEntity", std::string(type) + " name is empty");

    std::pair<typename MapT::iterator, bool> res = map.emplace(data.name, data);
    if (!res.second) return error("Scene", "addObject", std::string(type) + " name already used: " + data.name);

    return true;
  }

  template <typename T, typename MapT>
  bool getObjectByName(MapT& map, const std::string& name, T*& obj, const char* type) {
    typename MapT::iterator it = map.find(name);
    if (it == map.end()) return error("Scene", "getObjectByName", std::string(type) + " not found: " + name);
    obj = &it->second;
    return true;
	}

  std::map<std::string, Model>& getModels() { return models; }
	std::map<std::string, Camera>& getCameras() { return cameras; }
  std::map<std::string, Light>& getLights() { return lights; }
  std::map<std::string, Grid>& getGrids() { return grids; }
  std::map<std::string, TextureData>& getTextures() { return textures; }

	size_t getCameraCount() { return cameras.size(); }
  size_t getLightCount() { return lights.size(); }

  Camera* getActiveCamera();
  void setActiveCamera(unsigned int camIndex);

  void updateLights(int shaderProgram);
  void updateLightUniforms(int shaderProgram);

  bool bindTextureToModel(const std::string& modelName, unsigned int slot, const std::string& textureName, float mix);

private:
  std::string name;
  std::string activeCam;

  std::map<std::string, Model> models;
  std::map<std::string, Camera> cameras;
  std::map<std::string, Light> lights;
  std::map<std::string, Grid> grids;
  std::map<std::string, TextureData> textures;
};