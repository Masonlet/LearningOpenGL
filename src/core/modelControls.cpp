#define GLFW_INCLUDE_NONE 
#include <GLFW/glfw3.h> 

#include "core/modelControls.hpp"

static void processObject(InputManager* input, Vec3& pos, Vec3& rot, Vec3& size, float deltaTime){
  if (input->IsKeyDown(GLFW_KEY_UP))     pos.z += deltaTime;
  if (input->IsKeyDown(GLFW_KEY_DOWN))   pos.z -= deltaTime;
  if (input->IsKeyDown(GLFW_KEY_LEFT))   pos.x += deltaTime;
  if (input->IsKeyDown(GLFW_KEY_RIGHT))  pos.x -= deltaTime;
  if (input->IsKeyDown(GLFW_KEY_COMMA))  pos.y += deltaTime;
  if (input->IsKeyDown(GLFW_KEY_PERIOD)) pos.y -= deltaTime;

  const float scaleDelta = 0.02f;
  if (input->IsKeyDown(GLFW_KEY_E)) size *= (1.0f + scaleDelta);
  if (input->IsKeyDown(GLFW_KEY_Q)) size *= (1.0f - scaleDelta);

  const float rotationSpeed = 45.0f * deltaTime;
  if (input->IsKeyDown(GLFW_KEY_R)) rot.x += rotationSpeed;
  if (input->IsKeyDown(GLFW_KEY_F)) rot.x -= rotationSpeed;
  if (input->IsKeyDown(GLFW_KEY_T)) rot.y += rotationSpeed;
  if (input->IsKeyDown(GLFW_KEY_G)) rot.y -= rotationSpeed;
  if (input->IsKeyDown(GLFW_KEY_Y)) rot.z += rotationSpeed;
  if (input->IsKeyDown(GLFW_KEY_H)) rot.z -= rotationSpeed;
}

void handleModelInput(InputManager* input, float deltaTime, std::map<std::string, ModelData>& instances, int currentModel) {
  if(instances.empty()) return;

  std::map<std::string, ModelData>::iterator it = instances.begin();
  std::advance(it, currentModel);

  ModelData& instance = it->second;
  processObject(input, instance.pos, instance.rot, instance.size, deltaTime);

  instance.modelMatrix = Mat4::modelMatrix({.pos = {instance.pos, 0.0f}, .rot = instance.rot, .size = instance.size
  });
}


