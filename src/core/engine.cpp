#include <glad/glad.h> 

#include "core/engine.hpp"
#include "core/modelControls.hpp"

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
  if (!windowManager.createWindow(width, height, title)) {
    fprintf(stderr, "Failed to create window\n");
    return false;
  }

  glfwSetWindowUserPointer(windowManager.getWindow()->getGLFWwindow(), this);

#ifndef NDEBUG
  printf("[Debug] Opengl info\n");
  printf("[Debug] Vendor: %s\n", glGetString(GL_VENDOR));
  printf("[Debug] Renderer: %s\n", glGetString(GL_RENDERER));
  printf("[Debug] Version: %s\n", glGetString(GL_VERSION));
#endif

  setupShaders();

  lightManager.GetUniformLocations(currentProgram);
  return true;
}

void Engine::setupShaders() {
#ifndef NDEBUG
  printf("[setupShaders] Shader setup start time: %f\n", glfwGetTime());
#endif

  ShaderManager::Shader vert_shader{ "vertex_shader.glsl" };
  ShaderManager::Shader frag_shader{ "fragment_shader.glsl" };

  if (!shaderManager.createProgramFromFile("shader1", vert_shader, frag_shader))
    fprintf(stderr, "[setupShaders ERROR] %s\n", shaderManager.getLastError().c_str());

  currentProgram = shaderManager.getIDFromFriendlyName("shader1");
  if (currentProgram == 0) {
    fprintf(stderr, "[setupShaders ERROR] Shader program ID is 0\n");
    return;
  }
  renderer.setProgram(currentProgram);

  constexpr float bgR = 0.2f;
  constexpr float bgG = 0.2f;
  constexpr float bgB = 0.2f;
  constexpr float bgA = 1.0f;
  glClearColor(bgR, bgG, bgB, bgA);

#ifndef NDEBUG
  printf("[setupShaders] Shader setup finish time: %f\n", glfwGetTime());
#endif
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
#ifndef NDEBUG
    printf("[Tick WARN] deltaTime clamped to %.3f (was %.3f)\n", max_delta, rawdelta);
#endif
    rawdelta = max_delta;
  }

  lastTime = currenttime;
  deltaTime = smoothing_factor * deltaTime + (2.0f - smoothing_factor) * rawdelta;
}
  
bool Engine::setScene(const std::string& sceneIn) {
  if (!sceneManager.loadTxtScene(sceneIn))  return false;
  else                                      return true;
}
bool Engine::loadSceneMeshes() {
#ifndef NDEBUG
  printf("[loadSceneModels] Load start time: %f\n", glfwGetTime());
#endif
  const unsigned int shaderProgramID = renderer.getProgram();
  if (shaderProgramID == 0) {
    fprintf(stderr, "[Engine ERROR] No active shader program set before loading meshes.\n");
    return false;
  }

	std::map<std::string, ModelData>& modelData = sceneManager.getScene().getModelData();
	std::map<std::string, ModelData>::iterator it = modelData.begin();
  for(; it != modelData.end(); ++it) {
    ModelData& data = it->second;

    const MeshData* info{};
    if (!meshManager.findMesh(data.meshPath, info)) {
      MeshData tempData;
      if (!meshManager.loadMeshFile(data.meshPath, tempData, shaderProgramID) ||
        !meshManager.findMesh(data.meshPath, info)) {
        fprintf(stderr, "[Engine ERROR] Failed to load mesh: %s\n", data.meshPath.c_str());
        return false;
      }
    }
	}

#ifndef NDEBUG
  printf("[loadSceneModels] Load finish time: %f\n", glfwGetTime());
#endif
  return true;
}

void Engine::run() {
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
      const Vec3& a = transparentInstances[j]->position;
      const Vec3& b = transparentInstances[j + 1]->position;

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

    skyBox->position = cameraManager.getActiveCamera()->getPos();
    skyBox->modelMatrix = Mat4::modelMatrix({ { skyBox->position, 0.0f }, skyBox->rotation, skyBox->scale });
    
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