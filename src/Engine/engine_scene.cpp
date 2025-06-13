#include "engine.hpp"
#include "files.hpp"
#include "parser.hpp"
#include "factory.hpp"

#include <cstring>
#include <fstream>
#include <iomanip>

const unsigned char* parseColour(const unsigned char* linePtr, Vec3& assignedColour, ColourMode& colourMode) {
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

      assignedColour = colour;
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
    else if (strcmp(colourName, "Random") == 0) colourMode = ColourMode::Random;   
    else if (strcmp(colourName, "Rainbow") == 0) colourMode = ColourMode::VerticalGradient;
  }

  return linePtr;
}

bool Engine::createSceneFromName(const std::string& scene){
  std::string scenepath = "../assets/scenes/" + scene + ".txt";
  std::string src{};

  if (!loadFile(src, scenepath)) {
    fprintf(stderr, "[error] failed to load scene: %s\n", scenepath.c_str());
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
    char name[64];
    linePtr = parseToken(linePtr, reinterpret_cast<unsigned char*>(name), sizeof(name));
    if (!linePtr || strlen(name) == 0) {
      fprintf(stderr, "[SCENE ERROR] failed to parse name\n");
      break;
    }
    if(*linePtr == ',') ++linePtr;

    if(strcmp(name, "cubeGrid") == 0){
      // Grid count
      unsigned int count = 0;
      if(!(linePtr = parseStringUInt(linePtr, count))) {
        fprintf(stderr, "[SCENE ERROR] Failed to parse cubeGrid count");
        break;
      }

      // Grid spacing
      float spacing = 0.0f;
      if(!(linePtr = parseFloat(linePtr, spacing))){
        fprintf(stderr, "[SCENE ERROR] Failed to parse cubeGrid spacing");
        break;
      }
      if(*linePtr == ',') ++linePtr;

      // Start position
      Vec3 position;
      if(!(linePtr = parseVec3(linePtr, position))){
        fprintf(stderr, "[SCENE ERROR] Failed to parse cubeGrid position");
        break;
      }
      if(*linePtr == ',') ++linePtr;

      // Start rotation
      Vec3 rotation;
      if(!(linePtr = parseVec3(linePtr, rotation))){
        fprintf(stderr, "[SCENE ERROR] Failed to parse cubeGrid start position");
        break;
      }
      if(*linePtr == ',') ++linePtr;

      // Cube scale
      Vec3 scale;
      if(!(linePtr = parseVec3(linePtr, scale))){
        fprintf(stderr, "[SCENE ERROR] Failed to parse cubeGrid start position");
        break;
      }
      if(*linePtr == ',') ++linePtr;

      // Cube colour
      Vec3 assignedColour = {1.0f, 1.0f, 1.0f};
      ColourMode colourMode = ColourMode::PLYColour;
      linePtr = parseColour(linePtr, assignedColour, colourMode);

      if(!createCubeGrid(*this, "cube", 0, count, {spacing, spacing}, rotation, scale)){
        fprintf(stderr, "[SCENE ERROR] Failed to create squareGrid\n");
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
      unsigned int count = 0;
      if(!(linePtr = parseStringUInt(linePtr, count))) {
        fprintf(stderr, "[SCENE ERROR] Failed to parse squareGrid count");
        break;
      }

      // Grid spacing
      float spacing = 0.0f;
      if(!(linePtr = parseFloat(linePtr, spacing))){
        fprintf(stderr, "[SCENE ERROR] Failed to parse squareGrid spacing");
        break;
      }
      if(*linePtr == ',') ++linePtr;

      Vec3 start;
      if(!(linePtr = parseFloat(linePtr, start.x)) || !(linePtr = parseFloat(linePtr, start.y)) || !(linePtr = parseFloat(linePtr, start.z))){
        fprintf(stderr, "[SCENE ERROR] Failed to parse squareGrid start position");
        break;
      }
      if(*linePtr == ',') ++linePtr;

      // Start rotation
      Vec3 rotation;
      if(!(linePtr = parseFloat(linePtr, rotation.x)) || !(linePtr = parseFloat(linePtr, rotation.y)) || !(linePtr = parseFloat(linePtr, rotation.z))){
        fprintf(stderr, "[SCENE ERROR] Failed to parse squareGrid start position");
        break;
      }
      if(*linePtr == ',') ++linePtr;

      // Cube scale
      Vec3 scale;
      if(!(linePtr = parseFloat(linePtr, scale.x)) || !(linePtr = parseFloat(linePtr, scale.y)) || !(linePtr = parseFloat(linePtr, scale.z))){
        fprintf(stderr, "[SCENE ERROR] Failed to parse squareGrid start position");
        break;
      }
      if(*linePtr == ',') ++linePtr;

      // Cube colour
      Vec3 assignedColour = {1.0f, 1.0f, 1.0f};
      ColourMode colourMode = ColourMode::PLYColour;
      linePtr = parseColour(linePtr, assignedColour, colourMode);

      if(!createSquareGrid(*this, "square", 0, count, {spacing, spacing}, rotation, {scale.x, scale.y})){
        fprintf(stderr, "[SCENE ERROR] Failed to create squareGrid\n");
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
      char name[64];
      linePtr = parseToken(linePtr, reinterpret_cast<unsigned char*>(name), sizeof(name));
      if (!linePtr || strlen(name) == 0) {
        fprintf(stderr, "[SCENE ERROR] failed to parse path\n");
        break;
      }
      if(*linePtr == ',') ++linePtr;

      Vec3 position;
      if(!(linePtr = parseVec3(linePtr, position))){
        fprintf(stderr, "[SCENE ERROR] failed to parse position\n");
        break;
      }
      if(*linePtr == ',') ++linePtr;

      Vec3 rotation;
      if(!(linePtr = parseVec3(linePtr, rotation))){
        fprintf(stderr, "[SCENE ERROR] Failed to parse rotation\n");
        break;
      }
      if(*linePtr == ',') ++linePtr;

      Vec3 scale;
      if (!(linePtr = parseVec3(linePtr, scale))) {
        fprintf(stderr, "[SCENE ERROR] Failed to parse triangle size\n");
        break;
      }
      if(*linePtr == ',') ++linePtr;

      Vec3 assignedColour = {1.0f, 1.0f, 1.0f};
      ColourMode colourMode = ColourMode::PLYColour;
      linePtr = parseColour(linePtr, assignedColour, colourMode);

      std::string sharedName;
      bool skipCache = false;

      if (colourMode == ColourMode::Solid) {
        int r = static_cast<int>(assignedColour.x * 255.0f);
        int g = static_cast<int>(assignedColour.y * 255.0f);
        int b = static_cast<int>(assignedColour.z * 255.0f);
        sharedName = std::string(name) + "_solid_" + std::to_string(r) + "_" + std::to_string(g) + "_" + std::to_string(b);
        skipCache = true; // always bake solid-colored triangles fresh
      } else {
        sharedName = "triangle_shared";
      }

      ModelDrawInfo info;
      bool meshExists = !skipCache && getMeshManager()->FindDrawInfoByModelName(sharedName, info);

      if (!meshExists) {
        Vec4 bakedVertexColour = {assignedColour.x, assignedColour.y, assignedColour.z, 1.0f};
        if (!createTriangle(getMeshManager(), sharedName, {scale.x, scale.y}, bakedVertexColour)) {
          fprintf(stderr, "[SCENE ERROR] Failed to create triangle mesh: %s\n", sharedName.c_str());
          break;
        }

        if (!getMeshManager()->FindDrawInfoByModelName(sharedName, info)) {
          fprintf(stderr, "[SCENE ERROR] Mesh still not found after creation: %s\n", sharedName.c_str());
          break;
        }

        addModelInfo(sharedName, info);
      }

      std::string instanceName = std::string(name) + "_instance";
      Mat4 transform = Mat4::translation(position);
      if (!addInstance(instanceName, sharedName, transform)) {
        fprintf(stderr, "[SCENE ERROR] Failed to add triangle instance\n");
        break;
      }

      modelInstances[instanceName].colour = assignedColour;
      modelInstances[instanceName].colourMode = colourMode;

      p = reinterpret_cast<const unsigned char*>(lineEnd);
      continue;
    }
    // Load models from file
    else {
      //path
      char path[128];
      linePtr = parseToken(linePtr, reinterpret_cast<unsigned char*>(path), sizeof(path));
      if (!linePtr || strlen(path) == 0) {
        fprintf(stderr, "[SCENE ERROR] failed to parse path\n");
        break;
      }
      if(*linePtr == ',') ++linePtr;

      Vec3 position;
      if(!(linePtr = parseVec3(linePtr, position))){
        fprintf(stderr, "[SCENE ERROR] failed to parse position\n");
        break;
      }
      if(*linePtr == ',') ++linePtr;

      Vec3 rotation;
      if(!(linePtr = parseVec3(linePtr, rotation))){
        fprintf(stderr, "[SCENE ERROR] Failed to parse rotation\n");
         break;
      }
      if(*linePtr == ',') ++linePtr;

      Vec3 scale;
      if(!(linePtr = parseVec3(linePtr, scale))){
        fprintf(stderr, "[SCENE ERROR] Failed to parse scale\n");
        break;
      }
      if(*linePtr == ',') ++linePtr;

      Vec3 assignedColour = {1.0f, 1.0f, 1.0f};
      ColourMode colourMode = ColourMode::PLYColour;
      linePtr = parseColour(linePtr, assignedColour, colourMode);

      if(!loadModel(name, path, position, rotation, scale, assignedColour, colourMode)){
        fprintf(stderr, "[SCENE ERROR] loadModel failed: %s\n", path);
        continue;
      }

      p = reinterpret_cast<const unsigned char*>(lineEnd);
    }
  }

  return true;
}
bool Engine::saveScene(){
  std::string scenePath = "../assets/scenes/" + currentScene + ".txt";
  std::ofstream file(scenePath);

  if(!file.is_open()){
    fprintf(stderr, "[SAVE SCENE ERROR] Failed to open scene file for saving: %s\n", scenePath.c_str());
    return false;
  }

  file << std::fixed << std::setprecision(6);  
  for (std::map<std::string, ModelInstance>::const_iterator it = modelInstances.begin(); it != modelInstances.end(); ++it) {
    const std::string& name = it->first;

    const ModelInstance& instance = it->second;
    std::map<std::string, ModelDrawInfo>::const_iterator mesh = modelInfos.find(instance.path);
    if (mesh == modelInfos.end()) {
      fprintf(stderr, "[SAVE SCENE ERROR] Missing mesh for '%s'\n", name.c_str());
      continue;
    }

    file << name << ", " << instance.path << ", "
      << instance.position.x << " " << instance.position.y << " " << instance.position.z << ", "
      << instance.rotation.x << " " << instance.rotation.y << " " << instance.rotation.z << ", "
      << instance.scale.x    << " " << instance.scale.y    << " " << instance.scale.z;

    switch (instance.colourMode) {
      case ColourMode::Solid:
        file << ", " << instance.colour.x * 255.0f << " "
          << instance.colour.y * 255.0f << " "
          << instance.colour.z * 255.0f;
        break;
      case ColourMode::Random:
        file << ", Random"; break;
      case ColourMode::VerticalGradient:
        file << ", Rainbow"; break;
      case ColourMode::PLYColour:
      default:
        break;
    }

    file << "\n";
  }

  printf("[SCENE] %s Saved!\n", currentScene.c_str());
  return true;
}
