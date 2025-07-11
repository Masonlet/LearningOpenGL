#include "core/engine.hpp"
#include "core/modelControls.hpp"
#include "core/callbacks.hpp"
#include "lights/lightHelper.hpp"

constexpr int default_width{ 1920 };
constexpr int default_height{ 1200 };
constexpr int GL_MAJOR{ 3 };
constexpr int GL_MINOR{ 3 };

static void cleanupWindow(GLFWwindow* window) {
  if (window) glfwDestroyWindow(window);
}

Engine::Engine() :
  window{ nullptr },
  currentProgram{ 0 }, currentModel{ 0 }, currentLight{ 0 },
  height{ default_height }, width{ default_width },
  deltaTime{ 0.0f }, lastTime{ 0.0f },
  aspect{ 0.0f },
  wireframe{ false },
  renderer(), shaderManager(), vaoManager(), lightManager(), scene(), sceneLoader(scene, &renderer, &lightManager)
{
  window = WindowManager::initGL(width, height);
  if (!window) {
    fprintf(stderr, "failed to initialize opengl\n");
    return;
  }
  aspect = static_cast<float>(width) / static_cast<float>(height);

#ifndef ndebug
  printf("initgl complete time: %f\n", glfwGetTime());

  printf("opengl info:\n");
  printf("vendor: %s\n", glGetString(GL_VENDOR));
  printf("renderer: %s\n", glGetString(GL_RENDERER));
  printf("version: %s\n", glGetString(GL_VERSION));
#endif

  setupShaders();
  WindowManager::setupGLState();
  WindowManager::setCallbacks(window);

  glViewport(0, 0, width, height);
  glfwSetWindowUserPointer(window, this);
}

Engine::~Engine() {
  scene.clearModels(vaoManager);
  WindowManager::destroy(window);
}

void Engine::updateAspect(unsigned int width, unsigned int height) {
  this->width = width;
  this->height = height;
  this->aspect = static_cast<float>(width) / static_cast<float>(height);
}
void Engine::setupShaders() {
#ifndef ndebug
  printf("shader start time: %f\n", glfwGetTime());
#endif

  shaderManager.setBasePath("assets/shaders/");

  ShaderManager::Shader vert_shader{ "vertex_shader.glsl" };
  ShaderManager::Shader frag_shader{ "fragment_shader.glsl" };

  if (!shaderManager.createProgramFromFile("shader1", vert_shader, frag_shader))
    fprintf(stderr, "[setupShaders ERROR] %s\n", shaderManager.getLastError().c_str());

  unsigned int shaderID = shaderManager.getIDFromFriendlyName("shader1");
  if (shaderID == 0) {
    fprintf(stderr, "[setupShaders ERROR] Shader program ID is 0\n");
    return;
  }

#ifndef ndebug
  printf("shaders create time: %f\n", glfwGetTime());
#endif

  renderer.initialize(&shaderManager, &vaoManager);
  renderer.setProgram(shaderID);
}

void Engine::updateWireframe() {
  wireframe = !wireframe;
}

void Engine::updateDeltaTime(const float currenttime) {
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
    printf("[warn] large delta time clamped: %f\n", rawdelta);
#endif
    rawdelta = max_delta;
  }

  lastTime = currenttime;
  deltaTime = smoothing_factor * deltaTime + (2.0f - smoothing_factor) * rawdelta;
}

void Engine::run(const std::string& sceneIn) {
  const std::string sceneName = sceneIn.empty() ? "Default" : sceneIn;

  if (!sceneLoader.loadTxtScene(sceneName)) {
    fprintf(stderr, "[SCENE ERROR] Scene '%s' could not be loaded.\n", sceneName.c_str());
    return;
  }

  lightManager.GetUniformLocations(currentProgram);

  while (!glfwWindowShouldClose(window)) {	
    const float currenttime = static_cast<float>(glfwGetTime());

    updateDeltaTime(currenttime);
    handleModelInput(camera, window, deltaTime, scene.getModelInstances(), currentModel);
    renderFrame();

    glfwSwapBuffers(window); 
    glfwPollEvents();      
  }
}

void Engine::renderFrame() {
  glViewport(0, 0, width, height);
  glClearColor(0.2f, 0.2f, 0.2f, 1.0f);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  unsigned int shaderProgram = renderer.getProgram();
  if (shaderProgram != currentProgram) {
    glUseProgram(shaderProgram);
    currentProgram = shaderProgram;
    lightManager.GetUniformLocations(shaderProgram);
  }

  const Mat4 view = camera.LookAt();
  const Mat4 projection = camera.Perspective(aspect);
  const Vec3 eye = camera.Pos();
  renderer.updateCameraUniforms(eye, view, projection);

  lightManager.UpdateShaderUniforms(currentProgram);

  std::map<std::string, ModelInstance>::const_iterator it;
  for (it = scene.getModelInstances().begin(); it != scene.getModelInstances().end(); ++it) {
    const ModelInstance& currentModel = it->second;
    renderer.drawModel(currentModel, view, projection);
  }

  glBindVertexArray(0);
}


