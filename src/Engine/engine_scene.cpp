#include "engine.hpp"
#include "files.hpp"
#include "parser.hpp"

#include <cstring>
#include <fstream>
#include <iomanip>

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

    //path
    char path[128];
    linePtr = parseToken(linePtr, reinterpret_cast<unsigned char*>(path), sizeof(path));
    if (!linePtr || strlen(path) == 0) {
      fprintf(stderr, "[SCENE ERROR] failed to parse path\n");
      break;
    }
    if(*linePtr == ',') ++linePtr;

    //position
    float posx = 0.0f, posy = 0.0f, posz = 0.0f;
    if(!(linePtr = parseFloat(linePtr, posx))) {
      fprintf(stderr, "[SCENE ERROR] failed to parse position X\n");
      break;
    }
    if(!(linePtr = parseFloat(linePtr, posy))){
      fprintf(stderr, "[SCENE ERROR] failed to parse position Y\n");
      break;
    }
    if(!(linePtr = parseFloat(linePtr, posz))){
      fprintf(stderr, "[SCENE ERROR] failed to parse position Z\n");
      break;
    }
    if(*linePtr == ',') ++linePtr;

    //rotation
    float rotx = 0.0f, roty = 0.0f, rotz = 0.0f;
    if(!(linePtr = parseFloat(linePtr, rotx))){
      fprintf(stderr, "[SCENE ERROR] failed to parse rotation X\n");
      break;
    }
    if(!(linePtr = parseFloat(linePtr, roty))){
      fprintf(stderr, "[SCENE ERROR] failed to parse rotation Y\n");
      break;
    }
    if(!(linePtr = parseFloat(linePtr, rotz))){
      fprintf(stderr, "[SCENE ERROR] failed to parse rotation Z\n");
      break;
    }
    if(*linePtr == ',') ++linePtr;

    //scale
    float scalex = 1.0f, scaley = 1.0f, scalez = 1.0f;
    if(!(linePtr = parseFloat(linePtr, scalex))){
      fprintf(stderr, "[SCENE ERROR] failed to parse scale X\n");
      break;
    }
    if(!(linePtr = parseFloat(linePtr, scaley))){
      fprintf(stderr, "[SCENE ERROR] failed to parse scale Y\n");
      break;
    }
    if(!(linePtr = parseFloat(linePtr, scalez))){
      fprintf(stderr, "[SCENE ERROR] failed to parse scale Z\n");
      break;
    }
    if(*linePtr == ',') ++linePtr;

    Vec3 assignedColour = {1.0f, 1.0f, 1.0f};
    ColourMode colourMode = ColourMode::PLYColour;

    linePtr = skipWhitespace(linePtr);
    if (*linePtr != '\0' && *linePtr != '-') {
      Vec3 colour = { 1.0f, 1.0f, 1.0f };
      const unsigned char* original = linePtr;
      const unsigned char* temp = linePtr;

      if ((temp = parseFloat(linePtr, colour.x)) && 
        (temp = parseFloat(temp, colour.y)) &&
        (temp = parseFloat(temp, colour.z))) {
        if (colour.x > 1.0f || colour.y > 1.0f || colour.z > 1.0f) {
          colour.x /= 255.0f;
          colour.y /= 255.0f;
          colour.z /= 255.0f;
        }
        assignedColour = colour;
        colourMode = ColourMode::Solid;
        linePtr = temp;
      } else {
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
        else { colourMode = ColourMode::PLYColour; }
      } 
    }

    if(!loadModel(name, path, {posx, posy, posz}, {rotx, roty, rotz},{scalex, scaley, scalez}, assignedColour, colourMode)){
      fprintf(stderr, "[SCENE ERROR] loadModel failed: %s\n", path);
      continue;
    }

    p = reinterpret_cast<const unsigned char*>(lineEnd);
  }

  return true;
}
bool Engine::saveScene(){
  std::string scenePath = "../assets/scenes/" + currentScene + ".txt";
  std::ofstream file(scenePath);

  if(!file.is_open()){
    fprintf(stderr, "[SAVE SCENE ERROR] Failed to open scele file for saving: %s\n", scenePath.c_str());
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
