#include <glad/glad.h> 

#include "core/engine.hpp"
#include "core/modelControls.hpp"
#include "utils/log.hpp"

constexpr int default_width{ 1920 };
constexpr int default_height{ 1200 };

Engine::Engine() :
  currentProgram{ 0 }, currentModel{ 0 },
  deltaTime{ 0.0f }, lastTime{ 0.0f },
  wireframe{ false },
  sceneManager(meshManager, currentProgram, lightManager, cameraManager, textureManager),
  renderer(shaderManager, meshManager, textureManager){
}
Engine::~Engine() {
  inputManager = {};
  meshManager.Shutdown();
  windowManager.destroyWindow();
}

bool Engine::initialize(const unsigned int width, const unsigned int height, const char* title) {
  if (!windowManager.createWindow(width, height, title)) return false;
  glfwSetWindowUserPointer(windowManager.getWindow()->getGLFWwindow(), this);

  debugLog("Engine", "OpenGL Info", true);
	debugLog("Engine", "Version: " + std::string(reinterpret_cast<const char*>(glGetString(GL_VERSION))), true);
	debugLog("Engine", "Vendor: " + std::string(reinterpret_cast<const char*>(glGetString(GL_VENDOR))), true);
	debugLog("Engine", "Renderer: " + std::string(reinterpret_cast<const char*>(glGetString(GL_RENDERER))), true);

  setupShaders();

  lightManager.GetUniformLocations(currentProgram);
  return true;
}

bool Engine::setupShaders() {
	debugLog("setupShaders", "Shader setup start time: " + std::to_string(glfwGetTime()), true);

  ShaderManager::Shader vert_shader{ "vertex_shader.glsl" };
  ShaderManager::Shader frag_shader{ "fragment_shader.glsl" };

  if (!shaderManager.createProgramFromFile("shader1", vert_shader, frag_shader)) 
    return error("Engine", "setupShaders", "Failed to create shader program from file");

  currentProgram = shaderManager.getIDFromFriendlyName("shader1");
  if (currentProgram == 0) 
		return error("Engine", "setupShaders", "Shader program ID is 0 after creation");
  
  renderer.setProgram(currentProgram);

  constexpr float bgR = 0.2f;
  constexpr float bgG = 0.2f;
  constexpr float bgB = 0.2f;
  constexpr float bgA = 1.0f;
  glClearColor(bgR, bgG, bgB, bgA);

	debugLog("setupShaders", "Shader setup finish time: " + std::to_string(glfwGetTime()), true);
  return true;
}

void Engine::updateWireframe() {
  wireframe = !wireframe;
}

void Engine::tick(const float currenttime) {
  if (lastTime == 0.0f) {
    lastTime = currenttime;
    deltaTime = 0.0f;
    return;
  }

  float rawdelta = currenttime - lastTime;
  constexpr float max_delta = 0.1f;
  constexpr float smoothing_factor = 0.9f;

  if (rawdelta > max_delta) {
		debugLog("Tick", "deltaTime clamped to " + std::to_string(max_delta) + " (was " + std::to_string(rawdelta) + ")", true);
    rawdelta = max_delta;
  }

  lastTime = currenttime;
  deltaTime = smoothing_factor * deltaTime + (2.0f - smoothing_factor) * rawdelta;
}
  
bool Engine::setScene(const std::string& sceneIn) {
  return sceneManager.loadTxtScene(sceneIn); 
}
bool Engine::loadSceneMeshes() {
  if (sceneManager.getScene().getSceneName().empty())
    if (!sceneManager.loadTxtScene("Default"))
			return error("Engine", "loadSceneModels", "No scene loaded and failed to load default scene");

	debugLog("loadSceneModels", "Load scene models start time: " + std::to_string(glfwGetTime()), true);
  const unsigned int shaderProgramID = renderer.getProgram();
  if (shaderProgramID == 0) return error("Engine", "loadSceneMeshes", "No active shader program set before loading meshes");

	std::map<std::string, ModelData>& modelData = sceneManager.getScene().getModelData();
	std::map<std::string, ModelData>::iterator it = modelData.begin();
  for(; it != modelData.end(); ++it) {
    ModelData& data = it->second;

    const MeshData* info{};
    if (!meshManager.findMesh(data.meshPath, info)) {
      MeshData tempData;
      if (!meshManager.loadMeshFile(data.meshPath, tempData, shaderProgramID) ||
				  !meshManager.findMesh(data.meshPath, info)) 
        return error("Engine", "loadSceneModels", "Failed to load mesh: " + data.meshPath);
    }
	}

	debugLog("loadSceneModels", "Load finish time: " + std::to_string(glfwGetTime()), true);
  return true;
}

void Engine::run() {
	if (cameraManager.getCameraCount() == 0) {
		if (!cameraManager.addCamera(Camera()))	debugLog("SceneManger", " Failed to add fallback camera");
		else                                    debugLog("SceneManger", " Added fallback camera", true);
	}

  getWindowManager().switchActiveWindowVisibiltiy();

  while (!windowManager.getWindow()->shouldClose()) {	
    tick(static_cast<float>(glfwGetTime()));
    inputManager.Update(windowManager.getWindow()->getGLFWwindow());
    cameraManager.getActiveCamera()->processInputs(&inputManager, getDeltaTime());
    handleModelInput(&inputManager, getDeltaTime(), sceneManager.getScene().getModelData(), currentModel);
    renderFrame();

    windowManager.getWindow()->swapBuffers();
    windowManager.getWindow()->pollEvents();
  }
}

void Engine::renderFrame() {
  // Begin Frame
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  const Mat4 view = cameraManager.getActiveCamera()->LookAt();
  const Mat4 projection = cameraManager.getActiveCamera()->Perspective(windowManager.getWindow()->getAspect());
  const Vec3 eye = cameraManager.getActiveCamera()->getPos();
  renderer.updateCameraUniforms(eye, view, projection);

  lightManager.UpdateShaderUniforms(currentProgram);

  // Draw Frame
  ModelData* skyBox = nullptr;
  std::vector<const ModelData*> transparentInstances;
  std::map<std::string, ModelData>& instances = sceneManager.getScene().getModelData();
  for (std::map<std::string, ModelData>::iterator it = instances.begin(); it != instances.end(); ++it) {
    ModelData& instance = it->second;
    if (instance.name == "skybox") skyBox = &instance;

    if (instance.colour.w >= 1.0f) renderer.drawModel(instance, view, projection);
    else                           transparentInstances.push_back(&instance);
  }

  for (size_t i = 0; i < transparentInstances.size(); ++i) {
    for (size_t j = 0; j < transparentInstances.size() - i - 1; ++j) {
      const Vec3& a = transparentInstances[j]->pos;
      const Vec3& b = transparentInstances[j + 1]->pos;

      float distA = (a.x - eye.x) * (a.x - eye.x) + (a.y - eye.y) * (a.y - eye.y) + (a.z - eye.z) * (a.z - eye.z);
      float distB = (b.x - eye.x) * (b.x - eye.x) + (b.y - eye.y) * (b.y - eye.y) + (b.z - eye.z) * (b.z - eye.z);

      if (distA < distB) {
        const ModelData* temp = transparentInstances[j];
        transparentInstances[j] = transparentInstances[j + 1];
        transparentInstances[j + 1] = temp;
      }
    }
  }

  for (const ModelData* instance : transparentInstances) 
    renderer.drawModel(*instance, view, projection);

  glUniform1i(renderer.getIsSkyboxLocation(), GL_TRUE);
  if (skyBox) {
    skyBox->isVisible = true;

    skyBox->pos = cameraManager.getActiveCamera()->getPos();
    skyBox->modelMatrix = Mat4::modelMatrix({ { skyBox->pos, 0.0f }, skyBox->rot, skyBox->scale });
    
    int skyboxTextureID = textureManager.getTextureIDFromName(skyBox->name);

    glActiveTexture(GL_TEXTURE20);
    glBindTexture(GL_TEXTURE_CUBE_MAP, skyboxTextureID);
    
    const int skyboxTextureLocation = renderer.getSkyboxTextureLocation();
    glUniform1i(skyboxTextureLocation, 20);

    glEnable(GL_CULL_FACE);
    glCullFace(GL_FRONT);     
    glDepthMask(GL_FALSE);
    renderer.drawModel(*skyBox, view, projection);
    skyBox->isVisible = false;
    glDepthMask(GL_TRUE);
    glCullFace(GL_BACK);
  }
  glUniform1i(renderer.getIsSkyboxLocation(), GL_FALSE);

  // End Frame
  glBindVertexArray(0);
}

void Engine::incrementModel() {
  if (sceneManager.getScene().getModelData().empty()) return;
  currentModel = (currentModel + 1) % static_cast<unsigned int>(sceneManager.getScene().getModelData().size());
}

void Engine::decrementModel() {
  if (sceneManager.getScene().getModelData().empty()) return;
  currentModel = (currentModel == 0) ? static_cast<unsigned int>(sceneManager.getScene().getModelData().size() - 1)
                                     : currentModel - 1;
}