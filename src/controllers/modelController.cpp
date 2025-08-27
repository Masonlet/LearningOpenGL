#define GLFW_INCLUDE_NONE 
#include <GLFW/glfw3.h> 

#include "controllers/modelController.hpp"

void ModelController::update(Model& model, const InputManager& input, float deltaTime) {
  Transform& transform = model.transform;
  if (input.isKeyDown(GLFW_KEY_UP))     transform.pos.z += deltaTime;
  if (input.isKeyDown(GLFW_KEY_DOWN))   transform.pos.z -= deltaTime;
  if (input.isKeyDown(GLFW_KEY_LEFT))   transform.pos.x += deltaTime;
  if (input.isKeyDown(GLFW_KEY_RIGHT))  transform.pos.x -= deltaTime;
  if (input.isKeyDown(GLFW_KEY_COMMA))  transform.pos.y += deltaTime;
  if (input.isKeyDown(GLFW_KEY_PERIOD)) transform.pos.y -= deltaTime;

  const float scaleDelta = 0.02f;
  if (input.isKeyDown(GLFW_KEY_E)) transform.size *= (1.0f + scaleDelta);
  if (input.isKeyDown(GLFW_KEY_Q)) transform.size *= (1.0f - scaleDelta);

  const float rotationSpeed = 45.0f * deltaTime;
  if (input.isKeyDown(GLFW_KEY_R)) transform.rot.x += rotationSpeed;
  if (input.isKeyDown(GLFW_KEY_F)) transform.rot.x -= rotationSpeed;
  if (input.isKeyDown(GLFW_KEY_T)) transform.rot.y += rotationSpeed;
  if (input.isKeyDown(GLFW_KEY_G)) transform.rot.y -= rotationSpeed;
  if (input.isKeyDown(GLFW_KEY_Y)) transform.rot.z += rotationSpeed;
  if (input.isKeyDown(GLFW_KEY_H)) transform.rot.z -= rotationSpeed;
}
