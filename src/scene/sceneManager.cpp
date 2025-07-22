#include <glad/glad.h>

#include "scene/sceneManager.hpp"
#include "scene/sceneParser.hpp"

#include "utils/files.hpp"
#include "utils/parser.hpp"

#include "utils/grids.hpp"
#include "utils/primitives.hpp"

#include <fstream>
#include <iomanip>

SceneManager::SceneManager(Renderer* renderer, LightManager* lightManager, CameraManager* cameraManager)
  : scene(), renderer(renderer), lightManager(lightManager), cameraManager(cameraManager) {}

bool SceneManager::loadTxtScene(const std::string& sceneIn) {
#ifndef NDEBUG
  fprintf(stderr, "[SceneManager] Scene load start: %f\n", glfwGetTime());
#endif
  std::string scenepath = "assets/scenes/" + sceneIn + ".txt";
  std::string src{};

  if (!loadFile(src, scenepath)) {
    fprintf(stderr, "[createSceneFromName ERROR] failed to load scene: %s\n", scenepath.c_str());
    return false;
  }

  const unsigned char* p = reinterpret_cast<const unsigned char*>(src.c_str());
  while (*p) {
    const unsigned char* lineStart = p;
    const unsigned char* lineEnd = skipToNextLine(p);
    size_t lineLen = lineEnd - lineStart;
      
    // Trim newline characters
    while (lineLen > 0 && (lineStart[lineLen - 1] == '\n' || lineStart[lineLen - 1] == '\r'))
      --lineLen;

    // Empty line
    if (lineLen == 0) {
      p = lineEnd;
      continue;
    }   

    if (!processSceneLine(lineStart, lineLen)) {
      fprintf(stderr, "[SceneManager ERROR] Failed to process scene line: %.*s\n", (int)lineLen, lineStart);
      return false;
    }

    p = reinterpret_cast<const unsigned char*>(lineEnd);
  }

  scene.setSceneName(sceneIn);

  if (cameraManager->getCameraCount() == 0) {
    Camera defaultCam;
    defaultCam.SetPos({ 0.0f, 5.0f, 10.0f });
    defaultCam.SetYaw(-90.0f);
    defaultCam.SetPitch(0.0f);
    defaultCam.SetNear(0.1f);
    defaultCam.SetFar(10000.0f);

    if (!cameraManager->addCamera(defaultCam)) 
      fprintf(stderr, "[SceneManager WARNING] Failed to add fallback camera\n");
    else {
#ifndef NDEBUG
      fprintf(stderr, "[SceneManager DEBUG] No camera in scene, fallback camera added\n");
#endif
    }
  }
#ifndef NDEBUG
  fprintf(stderr, "[SceneManager] Scene load finish: %f\n", glfwGetTime());
#endif
  return true;
}
bool SceneManager::processSceneLine(const unsigned char* p, size_t lineLen) {
  const unsigned char* linePtr = p;
  if (*linePtr == '\0') return true;

  unsigned char name[64]{};
  linePtr = parseToken(linePtr, name, sizeof(name));
  const char* nameStr = reinterpret_cast<const char*>(name);

  if (!linePtr || strlen(nameStr) == 0) {
    fprintf(stderr, "[createSceneFromName ERROR] Failed to parse name\n");
    return false;
  }
  if (*linePtr == ',') ++linePtr;

  bool handled{ true };
  if (strcmp(nameStr, "model") == 0) handled = handleModelLine(linePtr);
  else if (strcmp(nameStr, "light") == 0) handled = handleLightLine(linePtr);
  else if (strcmp(nameStr, "camera") == 0) handled = handleCameraLine(linePtr);
  else if (strcmp(nameStr, "cubeGrid") == 0) handled = handleCubeGridLine(linePtr);
  else if (strcmp(nameStr, "squareGrid") == 0) handled = handleSquareGridLine(linePtr);
  else if (strcmp(nameStr, "triangle") == 0) handled = handleTriangleLine(linePtr);
  else if (strcmp(nameStr, "maze") == 0) handled = handleMazeLine(linePtr);
  else if (strcmp(nameStr, "mazeData") == 0) handled = handleMazeData(linePtr);
  else if (strcmp(nameStr, "comment") == 0 || nameStr[0] == '#') return true;
  return handled;
}

bool SceneManager::saveTxtScene() {
  std::string scenePath = "../../../assets/scenes/" + scene.getSceneName() + ".txt";
  std::ofstream file(scenePath);

  if (!file.is_open()) {
    fprintf(stderr, "[saveTxtScene ERROR] Failed to open scene file for saving: %s\n", scenePath.c_str());
    return false;
  }

  const std::map<std::string, ModelDrawInfo>& meshes = scene.getModelInfos();
  const std::map<std::string, ModelInstance>& instances = scene.getModelInstances();
  for (const std::pair<const std::string, ModelInstance>& entry : instances) {
    const std::string& name = entry.first;
    const ModelInstance& instance = entry.second;

    if (name.rfind("triangle_instance", 0) == 0 ||
      name.rfind("cube_instance_", 0) == 0 ||
      name.rfind("square_instance_", 0) == 0 ||
      name.rfind("maze_", 0) == 0) 
      continue;

    std::map<std::string, ModelDrawInfo>::const_iterator mesh = meshes.find(instance.path);
    if (mesh == meshes.end()) {
      fprintf(stderr, "[saveTxtScene ERROR] Missing mesh for '%s'\n", name.c_str());
      continue;
    }

    file << std::fixed << std::setprecision(6);
    file << "model, " << name << ", "
      << instance.path << ", "
      << instance.position.x << " " << instance.position.y << " " << instance.position.z << ", "
      << instance.rotation.x << " " << instance.rotation.y << " " << instance.rotation.z << ", "
      << instance.scale.x << " " << instance.scale.y << " " << instance.scale.z << ", ";

    switch (instance.colourMode) {
    case ColourMode::Solid: {
      int r = static_cast<int>(instance.colour.x * 255.0f);
      int g = static_cast<int>(instance.colour.y * 255.0f);
      int b = static_cast<int>(instance.colour.z * 255.0f);

      if (r == 255 && g == 0 && b == 0)       file << "Red";
      else if (r == 0 && g == 255 && b == 0)  file << "Green";
      else if (r == 0 && g == 0 && b == 255)  file << "Blue";
      else file << r << " " << g << " " << b;
      break;
    }
    case ColourMode::Random: file << ", Random"; break;
    case ColourMode::VerticalGradient: file << ", Rainbow"; break;
    case ColourMode::PLYColour:
    default: break;
    }

    file << "\n";
  }

  for (int i = 0; i < LightManager::NUMBEROFLIGHTS; ++i) {
    const Light& light = lightManager->theLights[i];

    if (light.param2.x == 0.0f) continue;

    file << "light, " << lightManager->getLightName(i) << ", "
      << light.position.x << " " << light.position.y << " " << light.position.z << " " << light.position.w << ", "
      << light.diffuse.x << " " << light.diffuse.y << " " << light.diffuse.z << " " << light.diffuse.w << ", "
      << light.atten.x << " " << light.atten.y << " " << light.atten.z << " " << light.atten.w << ", "
      << light.direction.x << " " << light.direction.y << " " << light.direction.z << " " << light.direction.w << ", "
      << light.param1.x << " " << light.param1.y << " " << light.param1.z << " " << light.param1.w << ", "
      << light.param2.x << " " << light.param2.y << " " << light.param2.z << " " << light.param2.w << "\n";
  }

  return true;
}

static void applyColourSettings(ModelInstance& instance, const Vec4& colour, const ColourMode& mode) {
  instance.colour = colour;
  instance.colourMode = mode;
}

bool SceneManager::handleModelLine(const unsigned char* p) {
  ParsedModel model{};
  PARSE_OR_FALSE(parseModel, model, "Failed to parse model");

  ModelDrawInfo drawInfo;
  drawInfo.meshPath = model.path;
  drawInfo.colour = model.colour;
  drawInfo.colourMode = model.colourMode;

  scene.addModelInfo(model.path, drawInfo);
  if (!renderer->getVAOManager()->LoadModelIntoVAO(model.path, drawInfo, renderer->getProgram())) {
    fprintf(stderr, "[SceneManager ERROR] Failed to load model: %s\n", model.path.c_str());
    return false;
  }
  scene.addInstance(model.meshName, model.path, Mat4::modelMatrix({ {model.position, 0.0 }, model.rotation, model.scale }));

  ModelInstance& instance = scene.getModelInstances()[model.meshName];
  applyColourSettings(instance, model.colour, model.colourMode);

  return true;
}
bool SceneManager::handleLightLine(const unsigned char* p) {
  ParsedLight lightData{};
  if (!(p = parseLight(p, lightData))) {
    fprintf(stderr, "[SceneManager ERROR] Failed to parse light\n");
    return false;
  }

  Light* light = lightManager->getLightByName(lightData.name);
  if (!light) {
    fprintf(stderr, "[SceneManager ERROR] Unable to store light: %s\n", lightData.name.c_str());
    return false;
  }

  light->position = { lightData.position, 1.0 };
  light->diffuse = lightData.diffuse;
  light->atten = lightData.atten;
  light->direction = lightData.direction;
  light->param1 = { lightData.type, lightData.param1 };
  light->param2 = lightData.param2;

  return true;
}
bool SceneManager::handleCameraLine(const unsigned char* p) {
  ParsedCamera cameraData{};
  if (!(p = parseCamera(p, cameraData))) {
    fprintf(stderr, "[SceneManager ERROR] Failed to parse camera\n");
    return false;
  }

  Camera cam;
  cam.SetYaw(cameraData.yaw);
  cam.SetPitch(cameraData.pitch);
  cam.SetPos(cameraData.position);
  cam.SetMoveSpeed(cameraData.speed);

  if (!cameraManager->addCamera(cam)) {
    fprintf(stderr, "[SceneManager ERROR] Could not add camera index %u\n", cameraData.index);
    return false;
  }

  return true;
}

bool SceneManager::handleSquareGridLine(const unsigned char* p) {
  ParsedGrid grid;
  PARSE_OR_FALSE(parseGrid, grid, "Failed to parse cubeGrid colour");

  if (!createSquareGrid(*this, "cube", 0, grid.layout.count, { grid.layout.spacing, grid.layout.spacing }, grid.layout.rotation, { grid.layout.scale.x, grid.layout.scale.y })) {
    fprintf(stderr, "[SceneManager ERROR] Failed to create cubeGrid\n");
    return false;
  }

  std::map<std::string, ModelInstance>& inst = scene.getModelInstances();
  std::map<std::string, ModelInstance>::iterator it = inst.begin();
  for (it; it != inst.end(); ++it) {
    const std::string& instanceName = it->first;
    ModelInstance& instance = it->second;

    if (instanceName.rfind("cube_instance_", 0) == 0) applyColourSettings(instance, grid.colour, grid.colourMode);
  }

  return true;
}
bool SceneManager::handleCubeGridLine(const unsigned char* p) {
  ParsedGrid grid;
  PARSE_OR_FALSE(parseGrid, grid, "Failed to parse cubeGrid colour");

  if (!createCubeGrid(*this, "cube", 0, grid.layout.count, { grid.layout.spacing, grid.layout.spacing }, grid.layout.rotation, grid.layout.scale)) {
    fprintf(stderr, "[SceneManager ERROR] Failed to create cubeGrid\n");
    return false;
  }

  std::map<std::string, ModelInstance>& inst = scene.getModelInstances();
  std::map<std::string, ModelInstance>::iterator it = inst.begin();
  for (it; it != inst.end(); ++it) {
    const std::string& instanceName = it->first;
    ModelInstance& instance = it->second;

    if (instanceName.rfind("cube_instance_", 0) == 0) applyColourSettings(instance, grid.colour, grid.colourMode);
  }

  return true;
}
bool SceneManager::handleTriangleLine(const unsigned char* p) {
  ParsedTriangle triangle{};
  if (!(p = parseTriangle(p, triangle))) {
    fprintf(stderr, "[SceneManager ERROR] Failed to parse triangle\n");
    return false;
  }

  std::string sharedName;
  bool skipCache = false;

  if (triangle.colourMode == ColourMode::Solid) {
    int r = static_cast<int>(triangle.colour.x * 255.0f);
    int g = static_cast<int>(triangle.colour.y * 255.0f);
    int b = static_cast<int>(triangle.colour.z * 255.0f);
    sharedName = std::string(triangle.meshName) + "_solid_" + std::to_string(r) + "_" + std::to_string(g) + "_" + std::to_string(b);
    skipCache = true; // always bake solid-colored triangles fresh
  }
  else {
    sharedName = "triangle_shared";
  }

  ModelDrawInfo info;
  bool meshExists = !skipCache && renderer->getVAOManager()->FindDrawInfoByModelName(sharedName, info);

  if (!meshExists) {
    Vec4 bakedVertexColour = { triangle.colour.x, triangle.colour.y, triangle.colour.z, 1.0f };
    if (!createTriangle(renderer->getVAOManager(), sharedName, renderer->getProgram(), { triangle.transform.scale.x, triangle.transform.scale.y }, bakedVertexColour)) {
      fprintf(stderr, "[SceneManager ERROR] Failed to create triangle mesh: %s\n", sharedName.c_str());
      return false;
    }

    if (!renderer->getVAOManager()->FindDrawInfoByModelName(sharedName, info)) {
      fprintf(stderr, "[SceneManager ERROR] Mesh still not found after creation: %s\n", sharedName.c_str());
      return false;
    }

    scene.addModelInfo(sharedName, info);
  }

  std::string instanceName = std::string(triangle.meshName) + "_instance";
  Mat4 transform = Mat4::translation(triangle.transform.position);
  if (!scene.addInstance(instanceName, sharedName, transform)) {
    fprintf(stderr, "[SceneManager ERROR] Failed to add triangle instance\n");
    return false;
  }

  scene.getModelInstances()[instanceName].colour = triangle.colour;
  scene.getModelInstances()[instanceName].colourMode = triangle.colourMode;
  return true;
}

bool SceneManager::handleMazeLine(const unsigned char* p) {
  ParsedMaze maze;
  PARSE_OR_FALSE(parseMaze, maze, "Failed to parse maze");

  pendingMaze = maze;
  return true;
}
bool SceneManager::handleMazeData(const unsigned char* p) {
  if (!pendingMaze.has_value()) {
    fprintf(stderr, "[SceneManager ERROR] Unexpected mazeData with no pending maze\n");
    return false;
  }

  if (!(parseMazeData(p, *pendingMaze))) {
    fprintf(stderr, "[SceneManager ERROR] Failed to parse mazeData\n");
    return false;
  }

  if (!buildMaze(*pendingMaze)) {
    fprintf(stderr, "[SceneManager ERROR] Failed to build maze\n");
    return false;
  }

  pendingMaze.reset();
  return true;
}


static bool addWall(Scene& scene, const ParsedMaze& maze, const std::string& wallName, const std::string& wallDirection, const Vec4& worldPos, const Vec4& wallPos, const Vec3& wallRot) {
  const std::string name = wallName + wallDirection;

  const Vec4 pos = worldPos - wallPos;
  const Vec3 rot = maze.rot + wallRot;
  const Transform transform{ pos, rot, {1.0f, 1.0f, 1.0f} };
  if (!scene.addInstance(name, maze.wallType, Mat4::modelMatrix(transform))) {
    fprintf(stderr, "[SceneLoader ERROR] Failed to add maze instance: %s\n", name.c_str());
    return false;
  }

  return true;
}
bool SceneManager::buildMaze(const ParsedMaze& maze) {
  for (const std::string& modelPath : { maze.wallType, maze.floorType }) {
    ModelDrawInfo existingInfo;
    if (!renderer->getVAOManager()->FindDrawInfoByModelName(modelPath, existingInfo)) {
      ModelDrawInfo drawInfo;
      drawInfo.meshPath = modelPath;
      drawInfo.colour = { 1.0f, 1.0f, 1.0f, 1.0f };
      drawInfo.colourMode = ColourMode::PLYColour;

      scene.addModelInfo(modelPath, drawInfo);
      if (!renderer->getVAOManager()->LoadModelIntoVAO(modelPath, drawInfo, renderer->getProgram())) {
        fprintf(stderr, "[SceneLoader ERROR] Failed to load maze model: %s\n", modelPath.c_str());
        return false;
      }
    }
  }

  for (size_t row = 0; row < maze.layout.size(); ++row) {
    for (size_t col = 0; col < maze.layout[row].size(); ++col) {
      if (!maze.layout[row][col]) continue;

      const Vec4 localPos = { static_cast<float>(col) * maze.spacing, 0.0f, -static_cast<float>(row) * maze.spacing, 1.0f };
      const Mat4 mazeMatrix = Mat4::modelMatrix({ {maze.pos, 0.0}, maze.rot, {1.0f, 1.0f, 1.0f} });
      const Vec4 worldPos = mazeMatrix * localPos;

      std::string floorName = maze.mazeName + "_" + std::to_string(row) + "_" + std::to_string(col);
      Transform floorTransform { worldPos, maze.rot, {1.0f, 1.0f, 1.0f} };
      if (!scene.addInstance(floorName, maze.floorType, Mat4::modelMatrix(floorTransform))) {
        fprintf(stderr, "[SceneLoader ERROR] Failed to add maze instance: %s\n", floorName.c_str());
        return false;
      }

      std::string wallName = maze.mazeName + "_" + std::to_string(row) + "_" + std::to_string(col);

      if (row == 0 || !maze.layout[row - 1][col]) 
        addWall(scene, maze, wallName, "_N", worldPos, { 0.0f, 0.0f, 0.0f, 0.0f }, { 0.0f, 0.0f, 0.0f });

      if (row + 1 >= maze.layout.size() || !maze.layout[row + 1][col]) 
        addWall(scene, maze, wallName, "_S", worldPos, { maze.spacing, 0.0f, maze.spacing, 0.0f }, { 0.0f, 180.0f, 0.0f });

      if (col == 0 || !maze.layout[row][col - 1])                         
        addWall(scene, maze, wallName, "_E", worldPos, { maze.spacing, 0.0f, 0.0f, 0.0f }, { 0.0f, 90.0f, 0.0f });

      if (col + 1 >= maze.layout[row].size() || !maze.layout[row][col + 1]) 
        addWall(scene, maze, wallName, "_W", worldPos, { 0.0f, 0.0f, maze.spacing, 0.0f }, { 0.0f, -90.0f, 0.0f });
    }
  }

  return true;
}