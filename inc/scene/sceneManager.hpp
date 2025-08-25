#pragma once

#include "scene/scene.hpp"
#include "graphics/meshManager.hpp"
#include "utils/parserObjects.hpp"
#include "utils/parser.hpp"

struct SceneManager {
  Scene scene;

  bool saveTxtScene();
  bool loadTxtScene(const std::string& name);

private:
  bool processSceneLine(const unsigned char*& p);
 
  template<typename T, typename MapT>
  bool parseAndAddObject(const unsigned char*& p, bool (*parseFN)(const unsigned char*&, T&), MapT& map, const char* type) {
    T obj{};
    if (!parseFN(p, obj)) return false;
    return scene.addObject(map, obj, type);
  }

  bool parseAndAddTexture(const unsigned char*& p, bool (*parseFN)(const unsigned char*&, BMPTexture&), const char* type);

  bool handleTextureConnectionLine(const unsigned char*& p);
};