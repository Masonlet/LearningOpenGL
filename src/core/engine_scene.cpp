#include "core/engine.hpp"
#include "utils/files.hpp"
#include "utils/parser.hpp"
#include "utils/factory.hpp"

#include <cstring>
#include <fstream>
#include <iomanip>

#define PARSE_OR_FAIL(parser, target, errorMsg) \
    if(!(linePtr = parser(linePtr, target))) { \
        fprintf(stderr, "[createSceneFromName ERROR]: %s\n", errorMsg); \
        break; \
    } \
    if(*linePtr == ',') ++linePtr;

static const unsigned char* parseColour(const unsigned char* linePtr, Vec4& assignedColour, ColourMode& colourMode) {
  linePtr = skipWhitespace(linePtr);
  if (*linePtr != '\0' && *linePtr != '-') {
    Vec3 colour = { 1.0f, 1.0f, 1.0f };
    const unsigned char* original = linePtr;

    if ((linePtr = parseVec3(linePtr, colour))) {
      if (colour.x > 1.0f || colour.y > 1.0f || colour.z > 1.0f) {
        colour.x /= 255.0f;
        colour.y /= 255.0f;
        colour.z /= 255.0f;
      }

      assignedColour = { colour, 0.0f };
      colourMode = ColourMode::Solid;
      return linePtr;
    } 

    linePtr = original;
    char colourName[64]{};
    linePtr = parseToken(linePtr, (unsigned char*)colourName, sizeof(colourName));


    if (strcmp(colourName, "Red") == 0) colour = { 1.0f, 0.0f, 0.0f };
    else if (strcmp(colourName, "Green") == 0) colour = { 0.0f, 1.0f, 0.0f };
    else if (strcmp(colourName, "Blue") == 0) colour = { 0.0f, 0.0f, 1.0f };
    else if (strcmp(colourName, "Yellow") == 0) colour = { 1.0f, 1.0f, 0.0f };
    else if (strcmp(colourName, "White") == 0) colour = { 1.0f, 1.0f, 1.0f };
    else if (strcmp(colourName, "Gray") == 0  || strcmp(colourName, "Grey") == 0) colour = { 0.5f, 0.5f, 0.5f };
    else if (strcmp(colourName, "Random") == 0) {
      colourMode = ColourMode::Random;
      return linePtr;
    }
    else if (strcmp(colourName, "Rainbow") == 0) {
      colourMode = ColourMode::VerticalGradient;
      return linePtr;
    }

    assignedColour = { colour, 0.0f };
    colourMode = ColourMode::Solid;
  }

  return linePtr;
}

bool Engine::createSceneFromName(const std::string& scene){
  std::string scenepath = "assets/scenes/" + scene + ".txt";
  std::string src{};

  if (!loadFile(src, scenepath)) {
    fprintf(stderr, "[createSceneFromName ERROR] failed to load scene: %s\n", scenepath.c_str());
    return false;
  } else 
  currentScene = scene;

  const unsigned char* p = reinterpret_cast<const unsigned char*>(src.c_str());
  while(*p){
    const char* lineStart = reinterpret_cast<const char*>(p);
    const char* lineEnd = reinterpret_cast<const char*>(skipToNextLine(p));

    size_t lineLen = lineEnd - lineStart;
    while (lineLen > 0 && (lineStart[lineLen - 1] == '\n' || lineStart[lineLen - 1] == '\r'))
      --lineLen;

    if (lineLen == 0) {
      p = reinterpret_cast<const unsigned char*>(lineEnd);
      continue;
    } 

    std::string rawLine(lineStart, lineLen);
    const unsigned char* linePtr = reinterpret_cast<const unsigned char*>(rawLine.c_str());
    linePtr = skipWhitespace(linePtr);
    if (*linePtr == '\0' || *linePtr == '\n') {
      linePtr = reinterpret_cast<const unsigned char*>(lineEnd);
      continue;
    }

    //Name 
    char name[64]{};
    linePtr = parseToken(linePtr, reinterpret_cast<unsigned char*>(name), sizeof(name));
    if (!linePtr || strlen(name) == 0) {
      fprintf(stderr, "[createSceneFromName ERROR] Failed to parse name\n");
      break;
    }
    if(*linePtr == ',') ++linePtr;

    if (strcmp(name, "model") == 0) {
      char meshName[64]{};
      linePtr = parseToken(linePtr, reinterpret_cast<unsigned char*>(meshName), sizeof(meshName));
      if (!linePtr || strlen(meshName) == 0) {
        fprintf(stderr, "[createSceneFromName ERROR] failed to parse path\n");
        break;
      }
      if (*linePtr == ',') ++linePtr;

      char path[128]{};
      linePtr = parseToken(linePtr, reinterpret_cast<unsigned char*>(path), sizeof(path));
      if (!linePtr || strlen(path) == 0) {
        fprintf(stderr, "[createSceneFromName ERROR] failed to parse path\n");
        break;
      }
      if (*linePtr == ',') ++linePtr;

      Vec3 temp;
      PARSE_OR_FAIL(parseVec3, temp, "Failed to parse position");
      Vec4 position{ temp, 0.0f };

      Vec3 rotation;
      PARSE_OR_FAIL(parseVec3, rotation, "Failed to parse rotation");

      Vec3 scale;
      PARSE_OR_FAIL(parseVec3, scale, "Failed to parse scale");

      Vec4 assignedColour = { 1.0f, 1.0f, 1.0f, 0.0f };
      ColourMode colourMode = ColourMode::PLYColour;
      linePtr = parseColour(linePtr, assignedColour, colourMode);

      if (!loadModel(meshName, path, position, rotation, scale, assignedColour, colourMode)) {
        fprintf(stderr, "[createSceneFromName ERROR] loadModel failed: %s\n", path);
        continue;
      }

      p = reinterpret_cast<const unsigned char*>(lineEnd);
    }
    else if(strcmp(name, "cubeGrid") == 0){
      unsigned int count = 0;
      PARSE_OR_FAIL(parseStringUInt, count, "Failed to parse cubeGrid count");

      // Grid spacing
      float spacing = 0.0f;
      PARSE_OR_FAIL(parseFloat, spacing, "Failed to parse cubeGrid spacing");

      // Start position
      Vec3 startPosition;
      PARSE_OR_FAIL(parseVec3, startPosition, "Failed to parse cubeGrid start position");

      // Start rotation
      Vec3 rotation;
      PARSE_OR_FAIL(parseVec3, rotation, "Failed to parse cubeGrid rotation");

      // Cube scale
      Vec3 scale;
      PARSE_OR_FAIL(parseVec3, scale, "Failed to parse cubeGrid scale");

      // Cube colour
      Vec4 assignedColour = {1.0f, 1.0f, 1.0f, 0.0f};
      ColourMode colourMode = ColourMode::Solid;
      if (!(linePtr = parseColour(linePtr, assignedColour, colourMode))) {
        fprintf(stderr, "[createSceneFromName ERROR] Failed to parse cubeGrid colour\n");
        break;
      }
      if (*linePtr == ',') ++linePtr;

      if (!createCubeGrid(*this, "cube", 0, count, { spacing, spacing }, rotation, scale)) {
        fprintf(stderr, "[createSceneFromName ERROR] Failed to create squareGrid\n");
        break;
      }

      for (std::map<std::string, ModelInstance>::iterator it = modelInstances.begin(); it != modelInstances.end(); ++it) {
        const std::string& instanceName = it->first;
        ModelInstance& instance = it->second;

        if (instanceName.rfind("square_instance_", 0) == 0) {
          instance.colour = assignedColour;
          instance.colourMode = colourMode;
        }
      }

      p = reinterpret_cast<const unsigned char*>(lineEnd);
      continue;
    }
    else if(strcmp(name, "squareGrid") == 0){
      // Grid count
      unsigned int count{ 0 };
      PARSE_OR_FAIL(parseStringUInt, count, "Failed to parse squareGrid count");

      // Grid spacing
      float spacing = 0.0f;
      PARSE_OR_FAIL(parseFloat, spacing, "Failed to parse squareGrid spacing");

      Vec3 start;
      PARSE_OR_FAIL(parseVec3, start, "Failed to parse squareGrid start position");

      // Start rotation
      Vec3 rotation;
      PARSE_OR_FAIL(parseVec3, rotation, "Failed to parse squareGrid rotation");

      // Cube scale
      Vec3 scale;
      PARSE_OR_FAIL(parseVec3, scale, "Failed to parse squareGrid size");

      // Cube colour
      Vec4 assignedColour = {1.0f, 1.0f, 1.0f, 0.0f};
      ColourMode colourMode = ColourMode::PLYColour;
      if (!(linePtr = parseColour(linePtr, assignedColour, colourMode))) {
        fprintf(stderr, "[createSceneFromName ERROR] Failed to parse squareGrid colour\n");
        break;
      }
      if (*linePtr == ',') ++linePtr;

      if (!createSquareGrid(*this, "square", 0, count, { spacing, spacing }, rotation, { scale.x, scale.y })) {
        fprintf(stderr, "[createSceneFromName ERROR] Failed to create squareGrid\n");
        break;
      }

      for (std::map<std::string, ModelInstance>::iterator it = modelInstances.begin(); it != modelInstances.end(); ++it) {
        const std::string& instanceName = it->first;
        ModelInstance& instance = it->second;

        if (instanceName.rfind("square_instance_", 0) == 0) {
          instance.colour = assignedColour;
          instance.colourMode = colourMode;
        }
      }

      p = reinterpret_cast<const unsigned char*>(lineEnd);
      continue;
    }
    else if(strcmp(name, "triangle") == 0){
      char meshName[64]{};
      linePtr = parseToken(linePtr, reinterpret_cast<unsigned char*>(meshName), sizeof(meshName));
      if (!linePtr || strlen(meshName) == 0) {
        fprintf(stderr, "[createSceneFromName ERROR] failed to parse path\n");
        break;
      }
      if(*linePtr == ',') ++linePtr;

      Vec3 temp;
      PARSE_OR_FAIL(parseVec3, temp, "Failed to parse position");
      Vec4 position{ temp, 0.0f };

      Vec3 rotation;
      PARSE_OR_FAIL(parseVec3, rotation, "Failed to parse rotation");

      Vec3 scale;
      PARSE_OR_FAIL(parseVec3, scale, "Failed to parse triangle size");

      Vec4 assignedColour = {1.0f, 1.0f, 1.0f, 0.0f};
      ColourMode colourMode = ColourMode::PLYColour;
      linePtr = parseColour(linePtr, assignedColour, colourMode);

      std::string sharedName;
      bool skipCache = false;

      if (colourMode == ColourMode::Solid) {
        int r = static_cast<int>(assignedColour.x * 255.0f);
        int g = static_cast<int>(assignedColour.y * 255.0f);
        int b = static_cast<int>(assignedColour.z * 255.0f);
        sharedName = std::string(meshName) + "_solid_" + std::to_string(r) + "_" + std::to_string(g) + "_" + std::to_string(b);
        skipCache = true; // always bake solid-colored triangles fresh
      } else {
        sharedName = "triangle_shared";
      }

      ModelDrawInfo info;
      bool meshExists = !skipCache && getMeshManager()->FindDrawInfoByModelName(sharedName, info);

      if (!meshExists) {
        Vec4 bakedVertexColour = {assignedColour.x, assignedColour.y, assignedColour.z, 1.0f};
        if (!createTriangle(getMeshManager(), sharedName, getProgram(), {scale.x, scale.y}, bakedVertexColour)) {
          fprintf(stderr, "[createSceneFromName ERROR] Failed to create triangle mesh: %s\n", sharedName.c_str());
          break;
        }

        if (!getMeshManager()->FindDrawInfoByModelName(sharedName, info)) {
          fprintf(stderr, "[createSceneFromName ERROR] Mesh still not found after creation: %s\n", sharedName.c_str());
          break;
        }

        addModelInfo(sharedName, info);
      }

      std::string instanceName = std::string(meshName) + "_instance";
      Mat4 transform = Mat4::translation(position);
      if (!addInstance(instanceName, sharedName, transform)) {
        fprintf(stderr, "[createSceneFromName ERROR] Failed to add triangle instance\n");
        break;
      }

      modelInstances[instanceName].colour = assignedColour;
      modelInstances[instanceName].colourMode = colourMode;

      p = reinterpret_cast<const unsigned char*>(lineEnd);
      continue;
    }
    else if (strcmp(name, "light") == 0) {
      unsigned int lightIndex = 0;
      PARSE_OR_FAIL(parseStringUInt, lightIndex, "Failed to parse light index");

      Vec4 position;
      PARSE_OR_FAIL(parseVec4, position, "Failed to parse light position");

      Vec4 diffuse;
      PARSE_OR_FAIL(parseVec4, diffuse, "Failed to parse light diffuse");

      Vec4 atten;
      PARSE_OR_FAIL(parseVec4, atten, "Failed to parse light attenuation");

      Vec4 direction;
      PARSE_OR_FAIL(parseVec4, direction, "Failed to parse light direction");

      Vec4 param1;
      PARSE_OR_FAIL(parseVec4, param1, "Failed to parse light param1");

      Vec4 param2;
      PARSE_OR_FAIL(parseVec4, param2, "Failed to parse light param2");

      Light& light = lightManager->theLights[lightIndex];
      light.position = position;
      light.diffuse = diffuse;
      light.atten = atten;
      light.direction = direction;
      light.param1 = param1;
      light.param2 = param2;

      p = reinterpret_cast<const unsigned char*>(lineEnd);
      continue;
    }
    else {
      fprintf(stderr, "[createSceneFromName ERROR] Invalid scene format");
    }
  }

  return true;
}
bool Engine::saveScene(){
  std::string scenePath = "assets/scenes/" + currentScene + ".txt";
  std::ofstream file(scenePath);

  if(!file.is_open()){
    fprintf(stderr, "[saveScene ERROR] Failed to open scene file for saving: %s\n", scenePath.c_str());
    return false;
  }

  file << std::fixed << std::setprecision(6);  
  for (std::map<std::string, ModelInstance>::const_iterator it = modelInstances.begin(); it != modelInstances.end(); ++it) {
    const std::string& name = it->first;

    const ModelInstance& instance = it->second;
    std::map<std::string, ModelDrawInfo>::const_iterator mesh = modelInfos.find(instance.path);
    if (mesh == modelInfos.end()) {
      fprintf(stderr, "[saveScene ERROR] Missing mesh for '%s'\n", name.c_str());
      continue;
    }

    file << "model, " << name << ", "
      << instance.path << ", "
      << instance.position.x << " " << instance.position.y << " " << instance.position.z << ", "
      << instance.rotation.x << " " << instance.rotation.y << " " << instance.rotation.z << ", "
      << instance.scale.x    << " " << instance.scale.y    << " " << instance.scale.z;

    switch (instance.colourMode) {
      case ColourMode::Solid:
        file << ", " << instance.colour.x * 255.0f << " " << instance.colour.y * 255.0f << " " << instance.colour.z * 255.0f;
        break;
      case ColourMode::Random:
        file << ", Random"; 
        break;
      case ColourMode::VerticalGradient:
        file << ", Rainbow"; 
        break;
      case ColourMode::PLYColour:
      default:
        break;
    }

    file << "\n";
  }

  for (int i = 0; i < LightManager::NUMBEROFLIGHTS; ++i) {
    const Light& light = lightManager->theLights[i];

    if (light.param2.x == 0.0f)
      continue;

    file << "light, " << i << ", "
      << light.position.x << " " << light.position.y << " " << light.position.z << " " << light.position.w << ", "
      << light.diffuse.x << " " << light.diffuse.y << " " << light.diffuse.z << " " << light.diffuse.w << ", "
      << light.atten.x << " " << light.atten.y << " " << light.atten.z << " " << light.atten.w << ", "
      << light.direction.x << " " << light.direction.y << " " << light.direction.z << " " << light.direction.w << ", "
      << light.param1.x << " " << light.param1.y << " " << light.param1.z << " " << light.param1.w << ", "
      << light.param2.x << " " << light.param2.y << " " << light.param2.z << " " << light.param2.w << "\n";
  }

  printf("[saveScene] %s Saved!\n", currentScene.c_str());
  return true;
}
