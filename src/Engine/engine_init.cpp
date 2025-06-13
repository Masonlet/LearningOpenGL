#include "engine.hpp"
#include "callbacks.hpp"

constexpr int default_width{1920};
constexpr int default_height{1080};

Engine::Engine() : 
  window{ nullptr }, 
  shaderManager{ new ShaderManager() }, 
  meshManager{ new VAOManager() }, 
  program{ 0 }, 
  vertex_buffer{ 0 }, 
  currentProgram{ 0 }, currentModel{0}, currentScene{""}, 
  height{ default_height }, width{ default_width }, 
  deltaTime{ 0.0f }, lastTime{ 0.0f },
  aspect{ 0.0f }, 
  wireframe{ false } {
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

  glViewport(0, 0, width, height);
  glfwSetWindowUserPointer(window, this);
}

Engine::~Engine() {
  if (program) glDeleteProgram(program);
  delete shaderManager;
  delete meshManager;
  clearModels();
  if(window) cleanupWindow(window);
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

  shaderManager->setBasePath("../assets/shaders/");

  ShaderManager::Shader vert_shader; 
  vert_shader.fileName = "vertex_shader.glsl";
  ShaderManager::Shader frag_shader; 
  frag_shader.fileName = "fragment_shader.glsl";

  if (!shaderManager->createProgramFromFile("shader1", vert_shader, frag_shader)) {
    printf("%s\n", shaderManager->getLastError().c_str());
  } else {
#ifndef ndebug
    printf("shaders create time: %f\n", glfwGetTime());
#endif
  }

  program = shaderManager->getIDFromFriendlyName("shader1");
  if(program == 0){
    fprintf(stderr, "[ERROR] Shader program ID is 0\n");
    return;
  }

  modelLocation = glGetUniformLocation(program, "model");
  modelViewLocation   = glGetUniformLocation(program, "view");
  modelProjectionLocation  = glGetUniformLocation(program, "proj");

  useOverrideColourLocation = glGetUniformLocation(program, "useOverrideColour");
  colourOverrideLocation = glGetUniformLocation(program, "colourOverride"); 
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
