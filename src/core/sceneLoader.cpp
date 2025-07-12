#include "utils/files.hpp"
#include "utils/parser.hpp"

#include "core/sceneParser.hpp"
#include "core/sceneLoader.hpp"

#include "utils/primitives.hpp"
#include "utils/grids.hpp"

#include <fstream>
#include <cstring>
#include <iomanip>

SceneLoader::SceneLoader(Scene& scene, Renderer* renderer, LightManager* lightManager) : scene(scene), renderer(renderer), lightManager(lightManager) {}

static void applyColourSettings(ModelInstance& instance, const Vec4& colour, ColourMode mode) {
  instance.colour = colour;
  instance.colourMode = mode;
}

bool SceneLoader::handleCubeGridLine(const unsigned char* p) {
  ParsedGrid grid;
  PARSE_OR_FALSE(parseCubeGrid, grid, "Failed to parse cubeGrid colour");

  if (!createCubeGrid(*this, "cube", 0, grid.layout.count, { grid.layout.spacing, grid.layout.spacing }, grid.layout.rotation, grid.layout.scale)) {
    fprintf(stderr, "[SceneLoader ERROR] Failed to create cubeGrid\n");
    return false;
  }

  std::map<std::string, ModelInstance>& inst = scene.getModelInstances();
  std::map<std::string, ModelInstance>::iterator it = inst.begin();
  for (it; it != inst.end(); ++it) {
    const std::string& instanceName = it->first;
    ModelInstance& instance = it->second;

    if (instanceName.rfind("cube_instance_", 0) == 0) 
      applyColourSettings(instance, grid.colour, grid.colourMode);
  }

  return true;
}
bool SceneLoader::handleSquareGridLine(const unsigned char* p) {
  ParsedGrid grid;
  PARSE_OR_FALSE(parseSquareGrid, grid, "Failed to parse squareGrid colour");

  if (!createSquareGrid(*this, grid.meshName, 0, grid.layout.count, { grid.layout.spacing, grid.layout.spacing }, grid.layout.rotation, { grid.layout.scale.x, grid.layout.scale.y })) {
    fprintf(stderr, "[createSceneFromName ERROR] Failed to create squareGrid\n");
    return false;
  }

  for (std::map<std::string, ModelInstance>::iterator it = scene.getModelInstances().begin(); it != scene.getModelInstances().end(); ++it) {
    const std::string& instanceName = it->first;
    ModelInstance& instance = it->second;

    if (instanceName.rfind("square_instance_", 0) == 0) 
      applyColourSettings(instance, grid.colour, grid.colourMode);
  }

  return true;
}
bool SceneLoader::handleTriangleLine(const unsigned char* p) {
  ParsedTriangle triangle{};
  if (!(p = parseTriangle(p, triangle))) {
    fprintf(stderr, "[createSceneFromName ERROR] Failed to parse triangle\n");
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
      fprintf(stderr, "[createSceneFromName ERROR] Failed to create triangle mesh: %s\n", sharedName.c_str());
      return false;
    }

    if (!renderer->getVAOManager()->FindDrawInfoByModelName(sharedName, info)) {
      fprintf(stderr, "[createSceneFromName ERROR] Mesh still not found after creation: %s\n", sharedName.c_str());
      return false;
    }

    scene.addModelInfo(sharedName, info);
  }

  std::string instanceName = std::string(triangle.meshName) + "_instance";
  Mat4 transform = Mat4::translation(triangle.transform.position);
  if (!scene.addInstance(instanceName, sharedName, transform)) {
    fprintf(stderr, "[createSceneFromName ERROR] Failed to add triangle instance\n");
    return false;
  }

  scene.getModelInstances()[instanceName].colour = triangle.colour;
  scene.getModelInstances()[instanceName].colourMode = triangle.colourMode;
  return true;
}

bool SceneLoader::handleModelLine(const unsigned char* p) {
  ParsedModel model{};
  PARSE_OR_FALSE(parseModel, model, "Failed to parse model");

  ModelDrawInfo drawInfo;
  drawInfo.meshPath = model.path;
  drawInfo.colour = model.colour;
  drawInfo.colourMode = model.colourMode;

  scene.addModelInfo(model.path, drawInfo);
  if (!renderer->getVAOManager()->LoadModelIntoVAO(model.path, drawInfo, renderer->getProgram())) {
    fprintf(stderr, "[SceneLoader ERROR] Failed to load model: %s\n", model.path.c_str());
    return false;
  }
  scene.addInstance(model.meshName, model.path, Mat4::modelMatrix({ model.position, model.rotation, model.scale }));

  ModelInstance& instance = scene.getModelInstances()[model.meshName];
  applyColourSettings(instance, model.colour, model.colourMode);

  return true;
}
bool SceneLoader::handleLightLine(const unsigned char* p) {
  ParsedLight lightData{};
  if (!(p = parseLight(p, lightData))) {
    fprintf(stderr, "[createSceneFromName ERROR] Failed to parse light\n");
    return false;
  }
  else {
    Light& light = lightManager->theLights[lightData.index];
    light.position = lightData.position;
    light.diffuse = lightData.diffuse;
    light.atten = lightData.atten;
    light.direction = lightData.direction;
    light.param1 = lightData.param1;
    light.param2 = lightData.param2;

    return true;
  }
}

bool SceneLoader::handleMazeLine(const unsigned char* p) {
  ParsedMaze maze;
  if (!parseMazeHeader(p, maze)) {
    fprintf(stderr, "[createSceneFromName ERROR] Failed to parse maze header\n");
    return false;
  }

  pendingMaze = maze;
  return true;
}
bool SceneLoader::buildMaze(Scene& scene, const ParsedMaze& maze) {
  const float tileSize = 1.0f;

  std::vector<std::string> modelsToLoad = { maze.wallType, maze.floorType };

  for (const std::string& modelPath : modelsToLoad) {
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
      const std::string& mesh = maze.layout[row][col] ? maze.wallType : maze.floorType;
      std::string instanceName = maze.mazeName + "_" + std::to_string(row) + "_" + std::to_string(col);

      Vec4 pos = { static_cast<float>(col), 0.0f, -static_cast<float>(row), 0.0f };
      if (!scene.addInstance(instanceName, mesh, Mat4::translation(pos))) {
        fprintf(stderr, "[SceneLoader ERROR] Failed to add maze instance: %s\n", instanceName.c_str());
        return false;
      }
    }
  }

  return true;
}
bool SceneLoader::handleMazeData(const unsigned char* p) {
  if (!pendingMaze.has_value()) {
    fprintf(stderr, "[SceneLoader ERROR] Unexpected mazeData with no pending maze\n");
    return false;
  }

  // Parse the maze layout data into the pendingMaze
  if (!(p = parseMazeData(p, *pendingMaze, scene))) {
    fprintf(stderr, "[SceneLoader ERROR] Failed to parse mazeData\n");
    return false;
  }

  // Build the actual maze in the scene
  if (!buildMaze(scene, *pendingMaze)) {
    fprintf(stderr, "[SceneLoader ERROR] Failed to build maze\n");
    return false;
  }

  pendingMaze.reset();
  return true;
}
  
bool SceneLoader::loadTxtScene(const std::string& sceneIn) {
  std::string scenepath = "assets/scenes/" + sceneIn + ".txt";
  std::string src{};

  if (!loadFile(src, scenepath)) {
    fprintf(stderr, "[createSceneFromName ERROR] failed to load scene: %s\n", scenepath.c_str());
    return false;
  }

  const unsigned char* p = reinterpret_cast<const unsigned char*>(src.c_str());
  while (*p) {
    const char* lineStart = reinterpret_cast<const char*>(p);
    const char* lineEnd = reinterpret_cast<const char*>(skipToNextLine(p));
    size_t lineLen = lineEnd - lineStart;

    while (lineLen > 0 && (lineStart[lineLen - 1] == '\n' || lineStart[lineLen - 1] == '\r'))
      --lineLen;

    if (lineLen == 0) {
      p = reinterpret_cast<const unsigned char*>(lineEnd);
      continue;
    }

    const unsigned char* linePtr = reinterpret_cast<const unsigned char*>(lineStart);
    linePtr = skipWhitespace(linePtr);
    if (*linePtr == '\0' || *linePtr == '\n') continue;

    //Name 
    char name[64]{};
    linePtr = parseToken(linePtr, reinterpret_cast<unsigned char*>(name), sizeof(name));
    if (!linePtr || strlen(name) == 0) {
      fprintf(stderr, "[createSceneFromName ERROR] Failed to parse name\n");
      return false;
    }
    if (*linePtr == ',') ++linePtr;

    bool handled{ false };
    if (strcmp(name, "model") == 0) handled = handleModelLine(linePtr);
    else if (strcmp(name, "cubeGrid") == 0) handled = handleCubeGridLine(linePtr);
    else if (strcmp(name, "squareGrid") == 0) handled = handleSquareGridLine(linePtr);
    else if (strcmp(name, "triangle") == 0) handled = handleTriangleLine(linePtr);
    else if (strcmp(name, "light") == 0) handled = handleLightLine(linePtr);
    else if (strcmp(name, "maze") == 0) handled = handleMazeLine(linePtr);
    else if (strcmp(name, "mazeData") == 0) handled = handleMazeData(linePtr);
    else fprintf(stderr, "[createSce  neFromName ERROR] Invalid scene format after line: '%.*s'\n", static_cast<int>(lineLen), lineStart);

    if (!handled) break;
    p = reinterpret_cast<const unsigned char*>(lineEnd);
  }

  return true;
}
bool SceneLoader::saveTxtScene(const std::string& sceneName) {
  std::string scenePath = "assets/scenes/" + sceneName + ".txt";
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
      << instance.scale.x << " " << instance.scale.y << " " << instance.scale.z;

    switch (instance.colourMode) {
    case ColourMode::Solid: file << ", " << instance.colour.x * 255.0f << " " << instance.colour.y * 255.0f << " " << instance.colour.z * 255.0f; break;
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

    file << "light, " << i << ", "
      << light.position.x << " " << light.position.y << " " << light.position.z << " " << light.position.w << ", "
      << light.diffuse.x << " " << light.diffuse.y << " " << light.diffuse.z << " " << light.diffuse.w << ", "
      << light.atten.x << " " << light.atten.y << " " << light.atten.z << " " << light.atten.w << ", "
      << light.direction.x << " " << light.direction.y << " " << light.direction.z << " " << light.direction.w << ", "
      << light.param1.x << " " << light.param1.y << " " << light.param1.z << " " << light.param1.w << ", "
      << light.param2.x << " " << light.param2.y << " " << light.param2.z << " " << light.param2.w << "\n";
  }

  printf("[saveTxtScene] %s Saved!\n", sceneName.c_str());
  return true;
}


