#pragma once

#include "utils/log.hpp"
#include "objects/modelData.hpp"
#include "objects/light.hpp"
#include "core/camera.hpp"
#include "utils/parserObjects.hpp"
#include "textures/bmp.hpp"
#include <map>
#include <string>

class Scene {
public:
  inline void setSceneName(const std::string& sceneName) { name = sceneName; }
  std::string& getSceneName() { return name; }

  template <typename T, typename MapT>
  bool addObject(MapT& map, const T& data, const char* typeName) {
    if (data.name.empty()) return error("Scene", "addEntity", std::string(typeName) + " name is empty");

    std::pair<typename MapT::iterator, bool> res = map.emplace(data.name, data);
    if (!res.second) return error("Scene", "addObject", std::string(typeName) + " name already used: " + data.name);

    return true;
  }

  std::map<std::string, ModelData>& getModels() { return models; }
	std::map<std::string, Camera>& getCameras() { return cameras; }
  std::map<std::string, Light>& getLights() { return lights; }
  std::map<std::string, BMPTexture>& getTextures() { return textures; }
  std::map<std::string, Grid>& getGrids() { return grids; }
  std::map<std::string, Triangle>& getTriangles() { return triangles; }
  std::map<std::string, Square>& getSquares() { return squares; }
  std::map<std::string, ParsedMaze>& getMaze() { return mazes; }

	size_t getCameraCount() { return cameras.size(); }
  size_t getLightCount() { return lights.size(); }

  Camera* getActiveCamera();
  void setActiveCamera(unsigned int camIndex);

  Light* getLightByName(std::string name);
  void updateLights(int shaderProgram);
  void updateLightUniforms(int shaderProgram);

	bool addTexture(const BMPTexture& data);
  unsigned int getTextureIDFromName(const std::string& textureFileName);
  bool bindTextureToModel(const std::string& modelName, unsigned int slot, const std::string& textureName, float mix);

  ParsedMaze* getMazeFromName(const std::string& name);

private:
  std::string name;
  std::string activeCam;

  std::map<std::string, ModelData> models;
  std::map<std::string, Camera> cameras;
  std::map<std::string, Light> lights;
	std::map<std::string, BMPTexture> textures;
  std::map<std::string, Grid> grids;
  std::map<std::string, Triangle> triangles;
  std::map<std::string, Square> squares;
  std::map<std::string, ParsedMaze> mazes;
};