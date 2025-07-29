#include <glad/glad.h> 

#include "core/engine.hpp"
#include "core/modelControls.hpp"

constexpr int default_width{ 1920 };
constexpr int default_height{ 1200 };

Engine::Engine() :
  currentProgram{ 0 }, currentModel{ 0 },
  deltaTime{ 0.0f }, lastTime{ 0.0f },
  wireframe{ false },
  sceneManager(&renderer, &lightManager, &cameraManager){
}
Engine::~Engine() {
  inputManager = {};
  vaoManager.Shutdown();
  windowManager.destroyWindow();
}

bool Engine::initialize(const unsigned int width, const unsigned int height, const char* title) {
  if (!windowManager.createWindow(width, height, title)) {
    fprintf(stderr, "Failed to create window\n");
    return false;
  }

  glfwSetWindowUserPointer(windowManager.getWindow()->getGLFWwindow(), this);

#ifndef ndebug
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
#ifndef ndebug
  printf("[setupShaders] Shader setup start time: %f\n", glfwGetTime());
#endif

  shaderManager.setBasePath("assets/shaders/");
  ShaderManager::Shader vert_shader{ "vertex_shader.glsl" };
  ShaderManager::Shader frag_shader{ "fragment_shader.glsl" };

  if (!shaderManager.createProgramFromFile("shader1", vert_shader, frag_shader))
    fprintf(stderr, "[setupShaders ERROR] %s\n", shaderManager.getLastError().c_str());

  currentProgram = shaderManager.getIDFromFriendlyName("shader1");
  if (currentProgram == 0) {
    fprintf(stderr, "[setupShaders ERROR] Shader program ID is 0\n");
    return;
  }
  glUseProgram(currentProgram);

  renderer.initialize(&shaderManager, &vaoManager);
  renderer.setProgram(currentProgram);

  constexpr float bgR = 0.2f;
  constexpr float bgG = 0.2f;
  constexpr float bgB = 0.2f;
  constexpr float bgA = 1.0f;
  glClearColor(bgR, bgG, bgB, bgA);

#ifndef ndebug
  printf("[setupShaders] Shader setup finish time: %f\n", glfwGetTime());
#endif
}

void Engine::updateWireframe() {
  wireframe = !wireframe;
}

void Engine::tick(const float currenttime) {
  constexpr float max_delta = 0.1f;
  constexpr float smoothing_factor = 0.9f;

  if (lastTime == 0.0f) {
    lastTime = currenttime;
    deltaTime = 0.0f;
    return;
  }

  float rawdelta = currenttime - lastTime;

  if (rawdelta > max_delta) {
#ifndef ndebug
    printf("[Tick WARN] deltaTime clamped to %.3f (was %.3f)\n", max_delta, rawdelta);
#endif
    rawdelta = max_delta;
  }

  lastTime = currenttime;
  deltaTime = smoothing_factor * deltaTime + (2.0f - smoothing_factor) * rawdelta;
}
  
bool Engine::setScene(const std::string& sceneIn) {
  if (!sceneManager.loadTxtScene(sceneIn))  return false;
  return true;
}
void Engine::run() {
  while (!windowManager.getWindow()->shouldClose()) {	
    tick(static_cast<float>(glfwGetTime()));
    inputManager.Update(windowManager.getWindow()->getGLFWwindow());
    cameraManager.getActiveCamera()->ProcessInputs(&inputManager, getDeltaTime());
    handleModelInput(&inputManager, getDeltaTime(), sceneManager.getScene().getModelInstances(), currentModel);
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
  const Vec3 eye = cameraManager.getActiveCamera()->Pos();
  renderer.updateCameraUniforms(eye, view, projection);

  lightManager.UpdateShaderUniforms(currentProgram);

  // Draw Frame
  for (const std::pair<const std::string, ModelInstance>& pair : sceneManager.getScene().getModelInstances())
    renderer.drawModel(pair.second, view, projection);
  
  // End Frame
  glBindVertexArray(0);
}

void Engine::incrementModel() {
  if (sceneManager.getScene().getModelInstances().empty()) return;
  currentModel = (currentModel + 1) % static_cast<unsigned int>(sceneManager.getScene().getModelInstances().size());
}

void Engine::decrementModel() {
  if (sceneManager.getScene().getModelInstances().empty()) return;
  currentModel = (currentModel == 0) 
    ? static_cast<unsigned int>(sceneManager.getScene().getModelInstances().size() - 1)
    : currentModel - 1;
}