#include <glad/glad.h> 

#include "core/engine.hpp"
#include "controllers/modelController.hpp"
#include "utils/log.hpp"
#include "parsers/plyParser.hpp"
#include "parsers/bmpParser.hpp"

constexpr int default_width{ 1920 };
constexpr int default_height{ 1200 };

bool Engine::initialize(const unsigned int width, const unsigned int height, const char* title) {
  if (!windowManager.createWindow(width, height, title)) return false;
  glfwSetWindowUserPointer(windowManager.getWindow()->getGLFWwindow(), this);

  if (!setupShaders()) return false;

  constexpr float bgR = 0.2f;
  constexpr float bgG = 0.2f;
  constexpr float bgB = 0.2f;
  constexpr float bgA = 1.0f;
  glClearColor(bgR, bgG, bgB, bgA);

  return true;
}

bool Engine::setupShaders() {
	debugLog("Engine", "setupShaders", "Shader setup start time: " + std::to_string(glfwGetTime()), true);

  if (!shaderManager.createProgramFromPaths("shader1", "vertex_shader.glsl", "fragment_shader.glsl"))
    return error("Engine", "setupShaders", "Failed to create shader program from file");

  if (!renderer.setProgram(shaderManager.getProgramID("shader1")))
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
bool Engine::loadSceneAssets() {
  return loadSceneMeshes() && loadSceneTextures();
}
bool Engine::loadSceneMeshes() {
  debugLog("Engine", "loadSceneMeshes", "Load scene models start time: " + std::to_string(glfwGetTime()), true);
  if (sceneManager.scene.getSceneName().empty())
    if (!sceneManager.loadTxtScene("Default"))
      return error("Engine", "loadSceneMeshes", "No scene loaded and failed to load default scene");

  const unsigned int shaderProgramID = renderer.getProgram();
  if (shaderProgramID == 0) return error("Engine", "loadSceneMeshes", "No active shader program set before loading meshes");

	std::map<std::string, Model>& modelData = sceneManager.scene.getObjects<Model>();
	std::map<std::string, Model>::iterator it = modelData.begin();
  for(; it != modelData.end(); ++it) {
    const std::string& path = it->second.meshPath;
    if (meshManager.findMesh(path)) continue;

    Mesh mesh;
    if (!parsePlyMesh(path, mesh))
      return false;

    if (!meshManager.uploadMeshToGPU(path, mesh, shaderProgramID))
      return error("Engine", "loadSceneMeshes", "Failed to load mesh: " + path);
	}

	renderer.updateLightCount(sceneManager.scene.getObjectCount<Light>());
  sceneManager.scene.updateLights(shaderProgramID);
  return debugLog("Engine", "loadSceneMeshes", "Load finish time: " + std::to_string(glfwGetTime()), true);
}
bool Engine::loadSceneTextures() {
  debugLog("Engine", "loadSceneTextures", "Load scene models start time: " + std::to_string(glfwGetTime()), true);
  if (sceneManager.scene.getSceneName().empty())
    if (!sceneManager.loadTxtScene("Default"))
      return error("Engine", "loadSceneMeshes", "No scene loaded and failed to load default scene");

  std::map<std::string, TextureData>& textureData = sceneManager.scene.getObjects<TextureData>();
  std::map<std::string, TextureData>::iterator it = textureData.begin();
  for (; it != textureData.end(); ++it) {
    const TextureData& texture = it->second;
    if (textureManager.findTexture(texture.name)) continue;

    if(!texture.isCube){
        Texture face;
        if (!parseBMP(texture.faces[0].c_str(), face)) return false;

        if (!textureManager.uploadTextureToGPU(texture.name, face, true))
          return error("Engine", "loadSceneMeshes", "Failed to load texture: " + texture.name);
    } else {
      Texture faces[6];
      for (int i = 0; i < 6; ++i)
        if (!parseBMP(texture.faces[i].c_str(), faces[i])) return false;

      if (!textureManager.uploadCubeTextureToGPU(texture.name, faces, true))
        return error("Engine", "loadSceneTextures", "Failed to upload cubemap: " + texture.name);
    }
  }
  return debugLog("Engine", "loadSceneTextures", "Load finish time: " + std::to_string(glfwGetTime()), true);
}

void Engine::run() {
  windowManager.switchActiveWindowVisibiltiy();

  while (!windowManager.getWindow()->shouldClose()) {	
    tick(static_cast<float>(glfwGetTime()));
    inputManager.Update(windowManager.getWindow()->getGLFWwindow());

    Model* model{ nullptr };
    if (!sceneManager.scene.getObjectByIndex<Model>(sceneManager.scene.modelController.currentModel, model))
      error("Engine", "run", "No active model found for selected model");
    else sceneManager.scene.modelController.update(*model, inputManager, deltaTime);

    renderFrame();
    windowManager.getWindow()->swapBuffers();
    windowManager.getWindow()->pollEvents();
  }
}

void Engine::renderFrame() {
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  Camera* cam{ nullptr };
  if (!sceneManager.scene.getObjectByIndex<Camera>(sceneManager.scene.cameraController.currentCamera, cam))
    error("Engine", "run", "No active camera found for selected camera");
  renderer.updateCameraUniforms(cam->pos, cam->LookAt(), cam->Perspective(windowManager.getWindow()->getAspect()));
  sceneManager.scene.cameraController.update(*cam, inputManager, deltaTime);

  sceneManager.scene.updateLightUniforms(renderer.getProgram());

  Model* skyBox{ nullptr };
  std::vector<Model*> transparentInstances;
  std::map<std::string, Model>& instances = sceneManager.scene.getObjects<Model>();
  for (std::map<std::string, Model>::iterator it = instances.begin(); it != instances.end(); ++it) {
    Model& instance = it->second;
    if (instance.name == "skybox") skyBox = &instance;

    if (instance.colour.w >= 1.0f) renderer.drawModel(meshManager, textureManager, sceneManager, instance);
    else                           transparentInstances.push_back(&instance);
  }

  for (size_t i = 0; i < transparentInstances.size(); ++i) {
    for (size_t j = 0; j < transparentInstances.size() - i - 1; ++j) {
      const Vec4& a = transparentInstances[j]->transform.pos;
      const Vec4& b = transparentInstances[j + 1]->transform.pos;

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
    renderer.drawModel(meshManager, textureManager, sceneManager, *instance);

  if (skyBox) {
    skyBox->transform.pos = { cam->pos, 0.0f };
		renderer.bindSkyboxTexture(textureManager.getTextureID(skyBox->name));

    renderer.setModelIsSkybox(true);
    skyBox->isVisible = true;
    glEnable(GL_CULL_FACE);
    glCullFace(GL_FRONT);     
    glDepthMask(GL_FALSE);
    renderer.drawModel(meshManager, textureManager, sceneManager, *skyBox);
    glDepthMask(GL_TRUE);
    glCullFace(GL_BACK);
    skyBox->isVisible = false;
    renderer.setModelIsSkybox(false);
  }

  glBindVertexArray(0);
}

