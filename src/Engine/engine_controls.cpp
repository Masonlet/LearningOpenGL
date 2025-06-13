#include "engine_controls.hpp"

void processObject(GLFWwindow* window, Vec3& pos, Vec3& rot, Vec3& scale, float deltaTime){
  if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS) pos.z += deltaTime;
  if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS) pos.z -= deltaTime;
  if (glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS) pos.x += deltaTime;
  if (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS) pos.x -= deltaTime; 
  if (glfwGetKey(window, GLFW_KEY_COMMA) == GLFW_PRESS) pos.y += deltaTime;
  if (glfwGetKey(window, GLFW_KEY_PERIOD) == GLFW_PRESS) pos.y -= deltaTime;

  const float scaleDelta = 0.02f;
  if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS) scale = scale * (1.0f + scaleDelta);
  if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS) scale = scale * (1.0f - scaleDelta);

  const float rotationSpeed = 45.0f * deltaTime;
  if (glfwGetKey(window, GLFW_KEY_R) == GLFW_PRESS) rot.x += rotationSpeed;
  if (glfwGetKey(window, GLFW_KEY_F) == GLFW_PRESS) rot.x -= rotationSpeed; 
  if (glfwGetKey(window, GLFW_KEY_T) == GLFW_PRESS) rot.y += rotationSpeed; 
  if (glfwGetKey(window, GLFW_KEY_G) == GLFW_PRESS) rot.y -= rotationSpeed; 
  if (glfwGetKey(window, GLFW_KEY_Y) == GLFW_PRESS) rot.z += rotationSpeed; 
  if (glfwGetKey(window, GLFW_KEY_H) == GLFW_PRESS) rot.z -= rotationSpeed;
}

void handleModelInput(Camera& camera, GLFWwindow* window, float deltaTime, std::map<std::string, ModelInstance>& instances, int currentModel) {
  if(instances.empty()) return;

  std::map<std::string, ModelInstance>::iterator it = instances.begin();
  std::advance(it, currentModel);

  ModelInstance& instance = it->second;
  camera.ProcessInputs(window, instance.position, instance.rotation, instance.scale, deltaTime);
  processObject(window, instance.position, instance.rotation, instance.scale, deltaTime);

  instance.modelMatrix = Mat4::modelMatrix({
    .position = instance.position,
    .rotation = instance.rotation,
    .scale = instance.scale
  });
}


