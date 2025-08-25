#include <glad/glad.h> 

#include "core/engine.hpp"
#include "core/modelControls.hpp"
#include "utils/log.hpp"

constexpr int default_width{ 1920 };
constexpr int default_height{ 1200 };

bool Engine::initialize(const unsigned int width, const unsigned int height, const char* title) {
  if (!windowManager.createWindow(width, height, title)) return false;
  glfwSetWindowUserPointer(windowManager.getWindow()->getGLFWwindow(), this);

  setupShaders();

  constexpr float bgR = 0.2f;
  constexpr float bgG = 0.2f;
  constexpr float bgB = 0.2f;
  constexpr float bgA = 1.0f;
  glClearColor(bgR, bgG, bgB, bgA);

  return true;
}

bool Engine::setupShaders() {
	debugLog("Engine", "setupShaders", "Shader setup start time: " + std::to_string(glfwGetTime()), true);

  ShaderManager::Shader vert_shader{ "vertex_shader.glsl" };
  ShaderManager::Shader frag_shader{ "fragment_shader.glsl" };

  if (!shaderManager.createProgramFromFile("shader1", vert_shader, frag_shader)) 
    return error("Engine", "setupShaders", "Failed to create shader program from file");

  if (!renderer.setProgram(shaderManager.getIDFromFriendlyName("shader1")))
    return error("Engine", "setupShaders", "");
  
  return debugLog("Engine", "setupShaders", "Shader setup finish time: " + std::to_string(glfwGetTime()), true);;
}

void Engine::tick(const float currentTime) {
  if (lastTime == 0.0f) {
    lastTime = currentTime;
    deltaTime = 0.0f;
    return;
  }

  float rawDelta = currentTime - lastTime;
  constexpr float maxDelta = 0.1f;
  constexpr float smoothingFactor = 0.9f;

  if (rawDelta > maxDelta) {
		debugLog("Engine", "Tick", "deltaTime clamped to " + std::to_string(maxDelta) + " (was " + std::to_string(rawDelta) + ")", true);
    rawDelta = maxDelta;
  }

  lastTime = currentTime;
  deltaTime = smoothingFactor * deltaTime + (1.0f - smoothingFactor) * rawDelta;
}
  
bool Engine::loadSceneMeshes() {
  if (sceneManager.scene.getSceneName().empty())
    if (!sceneManager.loadTxtScene("Default"))
      return error("Engine", "loadSceneMeshes", "No scene loaded and failed to load default scene");

	debugLog("Engine", "loadSceneMeshes", "Load scene models start time: " + std::to_string(glfwGetTime()), true);
  const unsigned int shaderProgramID = renderer.getProgram();
  if (shaderProgramID == 0) return error("Engine", "loadSceneMeshes", "No active shader program set before loading meshes");

	std::map<std::string, Model>& modelData = sceneManager.scene.getModels();
	std::map<std::string, Model>::iterator it = modelData.begin();
  for(; it != modelData.end(); ++it) {
    Model& model = it->second;
    if (!meshManager.findMesh(model.meshPath)) 
      if(!meshManager.UploadPathToGPU(model.meshPath, shaderProgramID)) 
        return error("Engine", "loadSceneMeshes", "Failed to load mesh: " + model.meshPath);
	}

	renderer.updateLightCount(sceneManager.scene.getLightCount());
  sceneManager.scene.updateLights(shaderProgramID);
  return debugLog("Engine", "loadSceneMeshes", "Load finish time: " + std::to_string(glfwGetTime()), true);;
}

void Engine::run() {
  windowManager.switchActiveWindowVisibiltiy();
  if(sceneManager.scene.getActiveCamera() == nullptr) sceneManager.scene.setActiveCamera(0);
  while (!windowManager.getWindow()->shouldClose()) {	
    tick(static_cast<float>(glfwGetTime()));
    inputManager.Update(windowManager.getWindow()->getGLFWwindow());
    sceneManager.scene.getActiveCamera()->processInputs(&inputManager, deltaTime);
    handleModelInput(&inputManager, deltaTime, sceneManager.scene.getModels(), currentModel);
    renderFrame();

    windowManager.getWindow()->swapBuffers();
    windowManager.getWindow()->pollEvents();
  }
}

void Engine::renderFrame() {
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	const Camera* cam = sceneManager.scene.getActiveCamera();
  renderer.updateCameraUniforms(cam->pos, cam->LookAt(), cam->Perspective(windowManager.getWindow()->getAspect()));

  sceneManager.scene.updateLightUniforms(renderer.getProgram());

  Model* skyBox{ nullptr };
  std::vector<Model*> transparentInstances;
  std::map<std::string, Model>& instances = sceneManager.scene.getModels();
  for (std::map<std::string, Model>::iterator it = instances.begin(); it != instances.end(); ++it) {
    Model& instance = it->second;
    if (instance.name == "skybox") skyBox = &instance;
    instance.modelMatrix = Mat4::modelMatrix({ { instance.pos, 0.0f }, instance.rot, instance.size });

    if (instance.colour.w >= 1.0f) renderer.drawModel(meshManager, sceneManager, instance);
    else                           transparentInstances.push_back(&instance);
  }

  for (size_t i = 0; i < transparentInstances.size(); ++i) {
    for (size_t j = 0; j < transparentInstances.size() - i - 1; ++j) {
      const Vec3& a = transparentInstances[j]->pos;
      const Vec3& b = transparentInstances[j + 1]->pos;

      const Vec3 eye = cam->pos;
      float distA = (a.x - eye.x) * (a.x - eye.x) + (a.y - eye.y) * (a.y - eye.y) + (a.z - eye.z) * (a.z - eye.z);
      float distB = (b.x - eye.x) * (b.x - eye.x) + (b.y - eye.y) * (b.y - eye.y) + (b.z - eye.z) * (b.z - eye.z);

      if (distA < distB) {
        Model* temp = transparentInstances[j];
        transparentInstances[j] = transparentInstances[j + 1];
        transparentInstances[j + 1] = temp;
      }
    }
  }

  for (Model* instance : transparentInstances) 
    renderer.drawModel(meshManager, sceneManager, *instance);

  if (skyBox) {
    skyBox->pos = sceneManager.scene.getActiveCamera()->pos;
    skyBox->modelMatrix = Mat4::modelMatrix({ { skyBox->pos, 0.0f }, skyBox->rot, skyBox->size });
		renderer.bindSkyboxTexture(sceneManager.scene.getTextureIDFromName(skyBox->name));

    renderer.setModelIsSkybox(true);
    skyBox->isVisible = true;
    glEnable(GL_CULL_FACE);
    glCullFace(GL_FRONT);     
    glDepthMask(GL_FALSE);
    renderer.drawModel(meshManager, sceneManager, *skyBox);
    glDepthMask(GL_TRUE);
    glCullFace(GL_BACK);
    skyBox->isVisible = false;
    renderer.setModelIsSkybox(false);
  }

  glBindVertexArray(0);
}

void Engine::incrementModel() {
  if (sceneManager.scene.getModels().empty()) return;
  currentModel = (currentModel + 1) % static_cast<unsigned int>(sceneManager.scene.getModels().size());
}

void Engine::decrementModel() {
  if (sceneManager.scene.getModels().empty()) return;
  currentModel = (currentModel == 0) ? static_cast<unsigned int>(sceneManager.scene.getModels().size() - 1)
                                     : currentModel - 1;
}