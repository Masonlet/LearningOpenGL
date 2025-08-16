#pragma once

#include "scene/scene.hpp"
#include "lights/lightManager.hpp"
#include "core/cameraManager.hpp"
#include "graphics/renderer.hpp"
#include "utils/parserObjects.hpp"

#include <optional>

class SceneManager {
public:
  SceneManager(Renderer* renderer, LightManager* lightManager, CameraManager* cameraManager);

  Scene& getScene() { return scene; }
  Renderer* getRenderer() const { return renderer; }

  bool loadTxtScene(const std::string& sceneName);
  bool saveTxtScene();

private:
  Scene scene;
  Renderer* renderer;
  LightManager* lightManager;
  CameraManager* cameraManager;

  std::optional<ParsedMaze> pendingMaze;

  bool processSceneLine(const unsigned char* p);

  bool handleSquareGridLine(const unsigned char* p);
  bool handleCubeGridLine(const unsigned char* p);
  bool handleTriangleLine(const unsigned char* p);

  bool handleModelLine(const unsigned char* p);
  bool handleLightLine(const unsigned char* p);
  bool handleCameraLine(const unsigned char* p);

  bool handleMazeLine(const unsigned char* p);
  bool handleMazeData(const unsigned char* p);
  bool buildMaze(const ParsedMaze& maze);
};