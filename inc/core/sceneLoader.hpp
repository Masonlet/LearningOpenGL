#pragma once

#include "core/scene.hpp"
#include "core/sceneParser.hpp"
#include "lights/lightManager.hpp"
#include "graphics/renderer.hpp"

#include <string>
#include <optional>

class SceneLoader {
public:
  SceneLoader(Scene& scene, Renderer* renderer, LightManager* lightManager);

  Scene& getScene() const { return scene; }
  Renderer* getRenderer() const { return renderer; }

  bool loadTxtScene(const std::string& sceneName);
  bool saveTxtScene(const std::string& sceneName);

private:
  Scene& scene;
  Renderer* renderer;
  LightManager* lightManager;

  std::optional<ParsedMaze> pendingMaze;

  bool handleCubeGridLine(const unsigned char* p);
  bool handleSquareGridLine(const unsigned char* p);
  bool handleTriangleLine(const unsigned char* p);

  bool handleModelLine(const unsigned char* p);
  bool handleLightLine(const unsigned char* p);

  bool handleMazeLine(const unsigned char* p);
  bool handleMazeData(const unsigned char* p);
  bool buildMaze(Scene& scene, const ParsedMaze& maze);
};