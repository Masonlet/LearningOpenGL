#pragma once

#include "scene/scene.hpp"
#include "lights/lightManager.hpp"
#include "core/cameraManager.hpp"
#include "graphics/renderer.hpp"
#include "utils/parserObjects.hpp"

#include <optional>

class SceneManager {
public:
  SceneManager(MeshManager& meshManager, Renderer& renderer, LightManager& lightManager, CameraManager& cameraManager, TextureManager& textureManager);

  Scene& getScene() { return scene; }

  bool loadTxtScene(const std::string& name);
  bool saveTxtScene();

private:
  Scene scene;
  Renderer& renderer;
  LightManager& lightManager;
  CameraManager& cameraManager;
	MeshManager& meshManager;
  TextureManager& textureManager;

  std::optional<ParsedMaze> pendingMaze;

  bool processSceneLine(const unsigned char*& p);

  bool handleSquareGridLine(const unsigned char*& p);
  bool handleCubeGridLine(const unsigned char*& p);
  bool handleTriangleLine(const unsigned char*& p);

  bool handleModelLine(const unsigned char*& p);
  bool handleLightLine(const unsigned char*& p);
  bool handleCameraLine(const unsigned char*& p);

  bool handleMazeLine(const unsigned char*& p);
  bool handleMazeData(const unsigned char*& p);
  bool buildMaze(const ParsedMaze& maze);

  bool handleTextureLine(const unsigned char*& p);
  bool handleTextureCubeLine(const unsigned char*& p);
};