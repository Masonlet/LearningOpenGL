#pragma once

#include "core/scene.hpp"
#include "lights/lightManager.hpp"
#include "graphics/renderer.hpp"

#include <string>

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

  bool handleModelLine(const unsigned char* p);
  bool handleCubeGridLine(const unsigned char* p);
  bool handleSquareGridLine(const unsigned char* p);
  bool handleTriangleLine(const unsigned char* p);
  bool handleLightLine(const unsigned char* p);
  const unsigned char* handleMazeLine(const unsigned char* p);
};