#pragma once

#include "scene/scene.hpp"
#include "graphics/meshManager.hpp"
#include "utils/parserObjects.hpp"

struct SceneManager {
  Scene scene;

  bool saveTxtScene();
  bool loadTxtScene(const std::string& name);

private:
  bool processSceneLine(const unsigned char*& p);
 
  bool handleModelLine(const unsigned char*& p);
  bool handleLightLine(const unsigned char*& p);
  bool handleCameraLine(const unsigned char*& p);

  bool handleTextureLine(const unsigned char*& p);
  bool handleTextureCubeLine(const unsigned char*& p);
  bool handleTextureConnectionLine(const unsigned char*& p);

  bool handleSquareGridLine(const unsigned char*& p);
  bool handleCubeGridLine(const unsigned char*& p);
  bool handleTriangleLine(const unsigned char*& p);

  bool handleMazeLine(const unsigned char*& p);
  bool handleMazeData(const unsigned char*& p);
  bool buildMaze(const ParsedMaze* maze);
};