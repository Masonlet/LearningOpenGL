#pragma once

struct Scene;

class SceneLoader {
public:
	bool loadScene(Scene& scene, const char* name);
	bool saveScene(const Scene& scene);

private:
  bool processSceneLine(Scene& scene, const unsigned char*& p);

  template<typename T>
  bool parseAndAddObject(Scene& scene, const unsigned char*& p, bool (*parseFN)(const unsigned char*&, T&), const char* type) {
    T obj{};
    if (!parseFN(p, obj)) return false;
    return scene.addObject<T>(obj, type);
  }
};