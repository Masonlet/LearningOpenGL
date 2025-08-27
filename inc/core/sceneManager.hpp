#pragma once

#include "core/scene.hpp"
#include "graphics/meshManager.hpp"
#include "parsers/parserFunctions.hpp"

struct SceneManager {
  Scene scene;

  bool saveTxtScene();
  bool loadTxtScene(const std::string& name);

private:
  bool processSceneLine(const unsigned char*& p);
 
  template<typename T>
  bool parseAndAddObject(const unsigned char*& p, bool (*parseFN)(const unsigned char*&, T&), const char* type) {
    T obj{};
    if (!parseFN(p, obj)) return false;
    return scene.addObject(scene.getObjects<T>(), obj, type);
  }
};