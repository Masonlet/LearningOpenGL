#pragma once

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

  bool addModel(const ModelData& data);
  std::map<std::string, ModelData>& getModels() { return models; }

  bool addCamera(const Camera& data);
	std::map<std::string, Camera>& getCameras() { return cameras; }
	size_t getCameraCount() { return cameras.size(); }
  Camera* getActiveCamera();
  void setActiveCamera(unsigned int camIndex);

  bool addLight(const Light& data);
  std::map<std::string, Light>& getLights() { return lights; }
  size_t getLightCount() { return lights.size(); }
  Light* getLightByName(std::string name);
  void updateLights(int shaderProgram);
  void updateLightUniforms(int shaderProgram);

	bool addTexture(const BMPTexture& data);
	std::map<std::string, BMPTexture>& getTextures() { return textures; }
  unsigned int getTextureIDFromName(const std::string& textureFileName);
  bool bindTextureToModel(const std::string& modelName, unsigned int slot, const std::string& textureName, float mix);

  bool addGrid(const Grid& data);
	std::map<std::string, Grid>& getGrids() { return grids; }

	bool addTriangle(const Triangle& data);
	std::map<std::string, Triangle>& getTriangles() { return triangles; }

  bool addSquare(const Square& data);
	std::map<std::string, Square>& getSquares() { return squares; }

  bool addMaze(const ParsedMaze& data);
  std::map<std::string, ParsedMaze>& getMaze() { return mazes; }
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