#include <glad/glad.h>

#include "scene/sceneManager.hpp"
#include "scene/sceneParser.hpp"

#include "utils/fileManager.hpp"
#include "utils/parser.hpp"

#include "models/grids.hpp"
#include "models/primitives.hpp"

#include <fstream>
#include <iomanip>

SceneManager::SceneManager(Renderer* renderer, LightManager* lightManager, CameraManager* cameraManager) 
  : renderer(renderer), lightManager(lightManager), cameraManager(cameraManager), scene() {}

bool SceneManager::loadTxtScene(const std::string& sceneIn) {
#ifndef NDEBUG
  fprintf(stderr, "[SceneManager] Scene load start: %f\n", glfwGetTime());
#endif

  const std::string scenepath = "assets/scenes/" + sceneIn + ".txt";
  std::string src{};
  if (!loadFile(src, scenepath)) {
    fprintf(stderr, "[createSceneFromName ERROR] failed to load scene: %s\n", scenepath.c_str());
    return false;
  }

  const unsigned char* p = reinterpret_cast<const unsigned char*>(src.c_str());
  while (*p) {
    const unsigned char* lineEnd = skipToNextLine(p);
      
    while (lineEnd > p && (lineEnd[-1] == '\n' || lineEnd[-1] == '\r')) 
      --lineEnd;

    if (lineEnd == p) {
      p = skipToNextLine(p);
      continue;
    }   

    if (!processSceneLine(p)) {
      const int len = static_cast<int>(lineEnd - p);
      fprintf(stderr, "[SceneManager ERROR] Failed to process scene line: %.*s\n", len, reinterpret_cast<const char*>(p));
      return false;
    }

    p = skipToNextLine(p);
  }

  scene.setSceneName(sceneIn);

  if (cameraManager->getCameraCount() == 0) {
    Camera defaultCam;
    defaultCam.setPos({ 0.0f, 5.0f, 10.0f });
    defaultCam.setYaw(-90.0f);
    defaultCam.setPitch(0.0f);
    defaultCam.setNear(0.1f);
    defaultCam.setFar(10000.0f);
    defaultCam.setType(0);

    if (!cameraManager->addCamera(defaultCam)) 
      fprintf(stderr, "[SceneManager WARNING] Failed to add fallback camera\n");
  }
#ifndef NDEBUG
  fprintf(stderr, "[SceneManager] Scene load finish: %f\n", glfwGetTime());
#endif
  return true;
}
bool SceneManager::processSceneLine(const unsigned char* p) {
  if (*p == '\0') return true;

  unsigned char name[64]{};
  const unsigned char* linePtr = parseToken(p, name, sizeof(name));
  const char* nameStr = reinterpret_cast<const char*>(name);

  if (!linePtr || strlen(nameStr) == 0) {
    fprintf(stderr, "[createSceneFromName ERROR] Failed to parse name\n");
    return false;
  }
  if (*linePtr == ',') ++linePtr;
  if (strcmp(nameStr, "comment") == 0 || nameStr[0] == '#') return true;

  bool handled{ true };
  if      (strcmp(nameStr, "model") == 0)      handled = handleModelLine(linePtr);
  else if (strcmp(nameStr, "light") == 0)      handled = handleLightLine(linePtr);
  else if (strcmp(nameStr, "camera") == 0)     handled = handleCameraLine(linePtr);
  else if (strcmp(nameStr, "cubeGrid") == 0)   handled = handleCubeGridLine(linePtr);
  else if (strcmp(nameStr, "squareGrid") == 0) handled = handleSquareGridLine(linePtr);
  else if (strcmp(nameStr, "triangle") == 0)   handled = handleTriangleLine(linePtr);
  else if (strcmp(nameStr, "maze") == 0)       handled = handleMazeLine(linePtr);
  else if (strcmp(nameStr, "mazeData") == 0)   handled = handleMazeData(linePtr);
  return handled;
}

bool SceneManager::saveTxtScene() {
  const std::string scenePath = "../../../assets/scenes/" + scene.getSceneName() + ".txt";
  std::ofstream file(scenePath);

  if (!file.is_open()) {
    fprintf(stderr, "[saveTxtScene ERROR] Failed to open scene file for saving: %s\n", scenePath.c_str());
    return false;
  }
  file << std::fixed << std::setprecision(3);

  file << "comment, name, pos(xyz), rot(yaw pitch), fov, nearPlane farPlane, camSpeed\n";
  const std::map<std::string, Camera>& cameras = cameraManager->getAllCameras();
  for (std::map<std::string, Camera>::const_iterator camIt = cameras.begin(); camIt != cameras.end(); ++camIt) {
    const std::string& name = camIt->first;
    const Camera& cam = camIt->second;
    const Vec3& pos = cam.getPos();

    const std::string camType = (cam.getType() == 0) ? "FreeCam" : 
                                (cam.getType() == 1) ? "DungeonCam" :
                                /*  .getType() == 2)*/ "ModernCam";
    
    file << "camera, " << name << ", " << camType << ", "
         << pos.x << " " << pos.y << " " << pos.z << ", "
         << cam.getYaw() << " " << cam.getPitch() << ", "
         << cam.getFov() << ", "
         << cam.getNearPlane() << " " << cam.getFarPlane() << ", "
         << cam.getMoveSpeed();

    if (cam.getType() != 0) file << ", " << cam.getMoveDistance();
    file << '\n';
  }

  file << "\ncomment, name, meshPath, pos(xyz), rot(xyz), scale(xyz), colour(Int, Named Coloured, Random, Rainbow, PLY), specular(rgb, power)\n";
  const std::map<std::string, const ModelDrawInfo*>& meshes = scene.getModelInfos();
  const std::map<std::string, ModelInstance>& instances = scene.getModelInstances();
  for (const std::pair<const std::string, ModelInstance>& entry : instances) {
    const std::string& name = entry.first;
    const ModelInstance& instance = entry.second;

    if (name.rfind("triangle_instance", 0) == 0 || name.rfind("cube_instance_", 0) == 0 || name.rfind("square_instance_", 0) == 0 || name.rfind("maze_", 0) == 0) 
      continue;

    std::map<std::string, const ModelDrawInfo*>::const_iterator mesh = meshes.find(instance.meshPath);
    if (mesh == meshes.end()) {
      fprintf(stderr, "[saveTxtScene ERROR] Missing mesh for '%s'\n", name.c_str());
      continue;
    }

    file << "model, " << name << ", "
         << instance.meshPath << ", "
         << instance.position.x << " " << instance.position.y << " " << instance.position.z << ", "
         << instance.rotation.x << " " << instance.rotation.y << " " << instance.rotation.z << ", "
         << instance.scale.x << " " << instance.scale.y << " " << instance.scale.z << ", ";

    switch (instance.colourMode) {
    case ColourMode::Solid: {
      int r = static_cast<int>(instance.colour.x * 255.0f);
      int g = static_cast<int>(instance.colour.y * 255.0f);
      int b = static_cast<int>(instance.colour.z * 255.0f);
      int a = static_cast<int>(instance.colour.w * 255.0f);

      if      (r == 255 && g == 0 && b == 0) file << "Red";
      else if (r == 0 && g == 255 && b == 0) file << "Green";
      else if (r == 0 && g == 0 && b == 255) file << "Blue";
      else                                   file << r << " " << g << " " << b << " " << a;
      break;
    }
    case ColourMode::Random:           file << "Random"; break;
    case ColourMode::VerticalGradient: file << "Rainbow"; break;
    case ColourMode::PLYColour:        file << "PLY"; break;
    default: break;
    }
    
    file << ", " << instance.specular.x << " " << instance.specular.y << " " << instance.specular.z << " " << instance.specular.w << "\n";
  }

  file << "\ncomment, name, type, pos (xyz), diffuse (rgba), atten (xyzw), direction, param1 (spotlight inner, spotight outer), param2 (on/off)\n";
  for (int i = 0; i < LightManager::NUMBEROFLIGHTS; ++i) {
    const Light& light = lightManager->theLights[i];
    if (light.param2.x == 0.0f) continue;
    
    const std::string camType = (light.param1.x == 0) ? "Point" :
                                (light.param1.x == 1) ? "Spot" :
                                /*     param1.x == 2)*/ "Directional";
    
    file << "light, " << lightManager->getLightName(i) << ", " << camType << ", "
         << light.position.x  << " " << light.position.y  << " " << light.position.z  << ", "
         << light.diffuse.x   << " " << light.diffuse.y   << " " << light.diffuse.z   << " " << light.diffuse.w   << ", "
         << light.atten.x     << " " << light.atten.y     << " " << light.atten.z     << " " << light.atten.w     << ", "
         << light.direction.x << " " << light.direction.y << " " << light.direction.z << " " << light.direction.w << ", "
         << light.param1.y    << " " << light.param1.z    << " " << light.param1.w    << ", "
         << light.param2.x    << " " << light.param2.y    << " " << light.param2.z    << " " << light.param2.w    << "\n";
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

  ModelDrawInfo tempInfo{};
  if (!renderer->getVAOManager()->LoadModelIntoVAO(model.path, tempInfo, renderer->getProgram())) {
    fprintf(stderr, "[SceneManager ERROR] Failed to load model: %s\n", model.path.c_str());
    return false;
  }

  const ModelDrawInfo* drawInfo = nullptr;
  if (!renderer->getVAOManager()->FindDrawInfoByModelName(model.path, drawInfo)) {
    fprintf(stderr, "[SceneManager ERROR] Draw info not found after loading\n");
    return false;
  }

  scene.addModelInfo(model.path, drawInfo);
  scene.addInstance(model.name, model.path, Mat4::modelMatrix({ {model.position, 0.0 }, model.rotation, model.scale }));

  ModelInstance& instance = scene.getModelInstances()[model.name];
  applyColourSettings(instance, model.colour, model.colourMode);
  instance.position = model.position;
  instance.rotation = model.rotation;
  instance.scale = model.scale;
  instance.specular = model.specular;
  instance.isVisible = model.isVisible;
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
  light->param1 = {lightData.param1Type, lightData.param1Direction};
  light->param2 = { static_cast<float>(lightData.isEnabled), 0.0f, 0.0f, 0.0f };
  return true;
}
bool SceneManager::handleCameraLine(const unsigned char* p) {
  ParsedCamera cameraData{};
  if (!(p = parseCamera(p, cameraData))) {
    fprintf(stderr, "[SceneManager ERROR] Failed to parse camera\n");
    return false;
  }

  Camera cam;
  cam.setName(cameraData.name);
  cam.setYaw(cameraData.yaw);
  cam.setPitch(cameraData.pitch);
  cam.setPos(cameraData.position);
  cam.setMoveSpeed(cameraData.speed);
	cam.setType(cameraData.type);
  cam.setFov(cameraData.fov);
  cam.setNear(cameraData.nearPlane);
  cam.setFar(cameraData.farPlane);
  if (cam.getType() != 0) cam.setMoveDistance(cameraData.moveDistance);

  if (!cameraManager->addCamera(cam)) {
    fprintf(stderr, "[SceneManager ERROR] Could not add camera\n");
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
    sharedName = std::string(triangle.name) + "_solid_" + std::to_string(r) + "_" + std::to_string(g) + "_" + std::to_string(b);
    skipCache = true;
  }
  else sharedName = "triangle_shared";

  const ModelDrawInfo* info = nullptr;
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

  std::string instanceName = std::string(triangle.name) + "_instance";
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

static bool addFloor(Scene& scene, const std::string& name, const std::string& mesh, const Vec4& worldPos, const Vec3& rotation) {
  Transform t{ worldPos, rotation, {1,1,1} };

  if (!scene.addInstance(name, mesh, Mat4::modelMatrix(t))) {
    fprintf(stderr, "[SceneLoader ERROR] Failed to add floor instance: %s\n", name.c_str());
    return false;
  }

  ModelInstance& instance = scene.getModelInstances()[name];
  instance.position = { worldPos.x, worldPos.y, worldPos.z };
  instance.rotation = rotation;
  return true;
}
static bool addWall(Scene& scene, const ParsedMaze& maze, const Vec4& worldPos, const Vec4& wallOffset, const Vec3& baseRot, bool& hasEntrance,
  const std::string& name, const std::string& wallName, const bool condition = true) {
  if (!condition) return false;

  std::string finalMesh = hasEntrance ? maze.exitType : maze.entranceType;
  if(finalMesh == maze.exitType)
    hasEntrance = true;

  const Vec4 pos = worldPos - wallOffset;
  const Vec3 rot = baseRot + maze.wallRot;

  for (unsigned int level = 0; level < maze.wallHeight; ++level) {
    Vec4 stackedPos = pos;
    stackedPos.y += static_cast<float>(level) * maze.spacing;

    std::string instanceName = wallName + "_" + std::to_string(level);
    const Transform transform{ stackedPos, rot, {1.0f, 1.0f, 1.0f} };
    if (!scene.addInstance(instanceName, name, Mat4::modelMatrix(transform))) {
      fprintf(stderr, "[SceneLoader ERROR] Failed to add maze instance: %s\n", wallName.c_str());
      return false;
    }

    ModelInstance& instance = scene.getModelInstances()[instanceName];
    instance.position = { stackedPos.x, stackedPos.y, stackedPos.z };
    instance.rotation = rot;
  }

  return true;
}
bool SceneManager::buildMaze(const ParsedMaze& maze) {
  for (const std::string& modelPath : { maze.floorType1, maze.floorType2, maze.floorType3, maze.floorType4, maze.floorType5, maze.floorType6, maze.floorWallType, maze.wallType1, maze.wallType2, maze.wallType3, maze.wallType4, maze.wallType5, maze.wallType6, maze.entranceType, maze.exitType, maze.exteriorWallType }) {
    const ModelDrawInfo* drawInfo = nullptr;
    if (!renderer->getVAOManager()->FindDrawInfoByModelName(modelPath, drawInfo)) {
      ModelDrawInfo tempInfo;
      tempInfo.meshPath = modelPath;

      if (!renderer->getVAOManager()->LoadModelIntoVAO(modelPath, tempInfo, renderer->getProgram())) {
        fprintf(stderr, "[SceneLoader ERROR] Failed to load maze model: %s\n", modelPath.c_str());
        return false;
      }

      if (!renderer->getVAOManager()->FindDrawInfoByModelName(modelPath, drawInfo)) {
        fprintf(stderr, "[SceneLoader ERROR] Draw info still missing after loading: %s\n", modelPath.c_str());
        return false;
      }
    }

    scene.addModelInfo(modelPath, drawInfo);
  }

  bool hasEntrance{ false };
  const Mat4 mazeMatrix = Mat4::modelMatrix({ {maze.pos, 0.0}, maze.rot, {1.0f, 1.0f, 1.0f} });
  for (size_t row = 0; row < maze.layout.size(); ++row) {
    for (size_t col = 0; col < maze.layout[row].size(); ++col) {
      const std::string iteration = std::to_string(row) + "_" + std::to_string(col);
      const Vec4 localPos = { static_cast<float>(col) * maze.spacing, 0.0f, -static_cast<float>(row) * maze.spacing, 1.0f };
      const Vec4 worldPos = mazeMatrix * localPos;

      if (maze.layout[row][col]) {
        bool wallExists{ false };

        const std::string wallVariants[] = { maze.wallType1, maze.wallType2, maze.wallType3, maze.wallType4, maze.wallType5, maze.wallType6 };
        std::string wallMesh = wallVariants[rand() % 6];

        const bool northCondition = (row == 0 || !maze.layout[row - 1][col]);
        wallExists = addWall(scene, maze, worldPos, { 0.0f,0.0f,0.0f,0.0f }, { 0.0f,0.0f,0.0f }, hasEntrance, wallMesh, maze.mazeName + "_wall_" + iteration + "_N", northCondition);

        const bool southCondition = (row + 1 >= maze.layout.size() || !maze.layout[row + 1][col]);
        wallExists = addWall(scene, maze, worldPos, { maze.spacing,0.0f,maze.spacing,0.0f }, { 0.0f,180.0f,0.0f }, hasEntrance, wallMesh, maze.mazeName + "_wall_" + iteration + "_S", southCondition);

        const bool eastCondition = (col == 0 || !maze.layout[row][col - 1]);
        wallExists = addWall(scene, maze, worldPos, { maze.spacing, 0.0f, 0.0f, 0.0f }, { 0.0f, 90.0f, 0.0f }, hasEntrance, wallMesh, maze.mazeName + "_wall_" + iteration + "_E", eastCondition);

        const bool westCondition = (col + 1 >= maze.layout[row].size() || !maze.layout[row][col + 1]);
        wallExists = addWall(scene, maze, worldPos, { 0.0f, 0.0f, maze.spacing, 0.0f }, { 0.0f, -90.0f, 0.0f }, hasEntrance, wallMesh, maze.mazeName + "_wall_" + iteration + "_W", westCondition);

        const std::string floorVariants[] = { maze.floorType1, maze.floorType2, maze.floorType3, maze.floorType4, maze.floorType5, maze.floorType6 };
        std::string floorMesh = floorVariants[rand() % 6];
        if (!addFloor(scene, maze.mazeName + "_floor_" + iteration, floorMesh, worldPos, maze.rot)) return false;

        if (maze.hasRoof) {
          Vec3 flippedRot = maze.rot + Vec3{ 180.0f, 0.0f, 0.0f };
          Vec4 roofPos = worldPos;
          roofPos.y += static_cast<float>(maze.wallHeight) * maze.spacing;
          roofPos.z -= maze.spacing;

          if (!addFloor(scene, maze.mazeName + "_roof_" + iteration, maze.floorWallType, roofPos, flippedRot)) return false;
        }
      }
      else {
        if (row == 0 || row == maze.layout.size() - 1 || col == 0 || col == maze.layout[row].size() - 1) {
          bool unusedFlag;

          if (row == 0)
            addWall(scene, maze, worldPos, { 0.0f, 0.0f, 0.0f, 0.0f }, { 0.0f, 0.0f, 0.0f }, unusedFlag, maze.exteriorWallType, maze.mazeName + "_exteriorwall_" + iteration + "_N");
          if (row == maze.layout.size() - 1)
            addWall(scene, maze, worldPos, { maze.spacing, 0.0f, maze.spacing, 0.0f }, { 0.0f, 180.0f, 0.0f }, unusedFlag, maze.exteriorWallType, maze.mazeName + "_exteriorwall_" + iteration + "_S");
          if (col == 0)
            addWall(scene, maze, worldPos, { maze.spacing, 0.0f, 0.0f, 0.0f }, { 0.0f, 90.0f, 0.0f }, unusedFlag, maze.exteriorWallType, maze.mazeName + "_exteriorwall_" + iteration + "_E");
          if (col == maze.layout[row].size() - 1)
            addWall(scene, maze, worldPos, { 0.0f, 0.0f, maze.spacing, 0.0f }, { 0.0f, -90.0f, 0.0f }, unusedFlag, maze.exteriorWallType, maze.mazeName + "_exteriorwall_" + iteration + "_W");
        }
      }
    }
  }
  return true;
}