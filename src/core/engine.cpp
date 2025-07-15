#include "core/engine.hpp"
#include "core/modelControls.hpp"
#include "core/callbacks.hpp"
#include "lights/lightHelper.hpp"

constexpr int default_width{ 1920 };
constexpr int default_height{ 1200 };

Engine::Engine() :
  window{ nullptr },
  currentProgram{ 0 }, currentModel{ 0 },
  height{ default_height }, width{ default_width },
  deltaTime{ 0.0f }, lastTime{ 0.0f },
  aspect{ static_cast<float>(width) / static_cast<float>(height) },
  wireframe{ false },
  sceneLoader(scene, &renderer, &lightManager)
{}
Engine::~Engine() {
  scene.clearModels(vaoManager);
  WindowManager::destroy(window);
}

bool Engine::initialize() {
  window = WindowManager::initGL(width, height);
  if (!window) {
    fprintf(stderr, "failed to initialize opengl\n");
    return false;
  }
  glfwSetWindowUserPointer(window, this);

#ifndef ndebug
  printf("[Debug] Opengl info:\n");
  printf("[Debug] Vendor: %s\n", glGetString(GL_VENDOR));
  printf("[Debug] Renderer: %s\n", glGetString(GL_RENDERER));
  printf("[Debug] Version: %s\n", glGetString(GL_VERSION));
#endif

  setupShaders();
  WindowManager::setupGLState();
  WindowManager::setCallbacks(window);

  lightManager.GetUniformLocations(currentProgram);
  return true;
}

void Engine::updateAspect(unsigned int width, unsigned int height) {
  this->width = width;
  this->height = height;
  this->aspect = static_cast<float>(width) / static_cast<float>(height);
  glViewport(0, 0, width, height);
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
    printf("[warn] deltaTime clamped to %.3f (was %.3f)\n", max_delta, rawdelta);
#endif
    rawdelta = max_delta;
  }

  lastTime = currenttime;
  deltaTime = smoothing_factor * deltaTime + (2.0f - smoothing_factor) * rawdelta;
}

bool Engine::setScene(const std::string& sceneIn = "Default") {
  const std::string sceneName = sceneIn.empty() ? "Default" : sceneIn;

  if (!sceneLoader.loadTxtScene(sceneName)) {
    fprintf(stderr, "[SCENE ERROR] Scene '%s' could not be loaded.\n", sceneName.c_str());
    return false;
  }

  return true;
}
void Engine::run() {
  while (!glfwWindowShouldClose(window)) {	
    tick(static_cast<float>(glfwGetTime()));
    input.Update(window);
    camera.ProcessInputs(&input, getDeltaTime());
    handleModelInput(&input, getDeltaTime(), scene.getModelInstances(), currentModel);
    renderFrame();

    glfwSwapBuffers(window); 
    glfwPollEvents();      
  }
}

void Engine::renderFrame() {
  // Begin Frame
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  const Mat4 view = camera.LookAt();
  const Mat4 projection = camera.Perspective(aspect);
  const Vec3 eye = camera.Pos();
  renderer.updateCameraUniforms(eye, view, projection);

  lightManager.UpdateShaderUniforms(currentProgram);

  // Draw Frame
  for (const std::pair<const std::string, ModelInstance>& pair : scene.getModelInstances())
    renderer.drawModel(pair.second, view, projection);
  
  // End Frame
  glBindVertexArray(0);
}

void Engine::incrementModel() {
  if (scene.getModelInstances().empty()) return;
  currentModel = (currentModel + 1) % static_cast<unsigned int>(scene.getModelInstances().size());
}

void Engine::decrementModel() {
  if (scene.getModelInstances().empty()) return;
  currentModel = (currentModel == 0) 
    ? static_cast<unsigned int>(scene.getModelInstances().size() - 1) 
    : currentModel - 1;
}

