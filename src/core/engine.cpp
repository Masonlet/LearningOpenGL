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
static void cleanupGL(const unsigned int program) {
  glBindBuffer(GL_ARRAY_BUFFER, 0);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
  glDeleteProgram(program);
  glBindVertexArray(0);
}

static GLFWwindow* createWindow(const int& width, const int& height, const char* title) {
#ifndef NDEBUG
  fprintf(stderr, "createWindow Start Time: %f\n", glfwGetTime());
#endif

  //glfwWindowHint(GLFW_DECORATED, GLFW_FALSE);
  //glfwWindowHint(GLFW_VISIBLE, GLFW_FALSE);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);

  GLFWwindow* window = glfwCreateWindow(width, height, title, nullptr, nullptr);
  if (!window)
    return nullptr;

  glfwMakeContextCurrent(window);
  glfwSwapInterval(1); // Enables VSYNC
  glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

#ifndef NDEBUG
  fprintf(stderr, "createWindow Finish Time: %f\n", glfwGetTime());
#endif
  return window;
}

GLFWwindow* initGL(unsigned int width, unsigned int height) {
  glfwSetErrorCallback(error_callback);
  if (!glfwInit()) {
    fprintf(stderr, "Failed to initialize GFLW\n");
    return nullptr;
  }

  GLFWwindow* window = createWindow(width, height, "Mason LEtoile 1146210");
  if (!window) {
    fprintf(stderr, "Failed to initialize window\n");
    return nullptr;
  }

  if (!gladLoadGLLoader(reinterpret_cast<GLADloadproc>(glfwGetProcAddress))) {
    fprintf(stderr, "Failed to initialize GLAD\n");
    cleanupWindow(window);
    return nullptr;
  }

  glViewport(0, 0, width, height);
  return window;
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
  window = initGL(width, height);
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
  setupGLState();
  setCallbacks();

  glViewport(0, 0, width, height);
  glfwSetWindowUserPointer(window, this);
}

Engine::~Engine() {
  scene.clearModels(vaoManager);
  if (window) cleanupWindow(window);
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

void Engine::setupGLState() {
  glEnable(GL_DEPTH_TEST);
  glDepthFunc(GL_LESS);

  //glEnable(GL_CULL_FACE);
  glDisable(GL_CULL_FACE);
  glCullFace(GL_BACK);
  glFrontFace(GL_CCW);

  glEnable(GL_BLEND);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
}

void Engine::setCallbacks() {
  glfwSetKeyCallback(window, key_callback);
  glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
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

void Engine::handleInputs() {
  handleModelInput(camera, window, deltaTime, scene.getModelInstances(), currentModel);
}

void Engine::run(const std::string& scene) {
  const std::string sceneName = scene.empty() ? "Default" : scene;

  if (!sceneLoader.loadTxtScene(sceneName)) {
    fprintf(stderr, "[SCENE ERROR] Scene '%s' could not be loaded.\n", sceneName.c_str());
    return;
  }

  lightManager.GetUniformLocations(currentProgram);

  while (!glfwWindowShouldClose(window)) {	
    const float currenttime = static_cast<float>(glfwGetTime());

    updateDeltaTime(currenttime);
    handleInputs();
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


