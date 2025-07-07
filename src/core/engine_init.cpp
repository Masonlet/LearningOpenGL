#include "core/engine.hpp"
#include "core/callbacks.hpp"

constexpr int default_width{1920};
constexpr int default_height{1200};

Engine::Engine() : 
  window{ nullptr }, 
  shaderManager{ new ShaderManager() }, meshManager{ new VAOManager() }, lightManager{new LightManager()},
  program{ 0 }, 
  currentProgram{ 0 }, currentModel{ 0 }, currentScene{ "" }, currentLight{ 0 },
  height{ default_height }, width{ default_width }, 
  deltaTime{ 0.0f }, lastTime{ 0.0f },
  aspect{ 0.0f }, 
  wireframe{ false },
  modelLocation{ 0 }, modelViewLocation{ 0 }, modelProjectionLocation{ 0 }, modelInverseTransposeLocation{ 0 },
  useOverrideColourLocation{ 0 }, colourOverrideLocation { 0 } {
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
  clearModels();
  delete shaderManager;
  delete meshManager;
  if (program) glDeleteProgram(program);
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

  shaderManager->setBasePath("assets/shaders/");

  ShaderManager::Shader vert_shader; 
  vert_shader.fileName = "vertex_shader.glsl";
  ShaderManager::Shader frag_shader; 
  frag_shader.fileName = "fragment_shader.glsl";

  if (!shaderManager->createProgramFromFile("shader1", vert_shader, frag_shader)) {
   fprintf(stderr, "[setupShaders ERROR] %s\n", shaderManager->getLastError().c_str());
  } else {
#ifndef ndebug
    printf("shaders create time: %f\n", glfwGetTime());
#endif
  }

  program = shaderManager->getIDFromFriendlyName("shader1");
  if(program == 0){
    fprintf(stderr, "[setupShaders ERROR] Shader program ID is 0\n");
    return;
  }

  modelLocation = glGetUniformLocation(program, "mModel");
  modelViewLocation   = glGetUniformLocation(program, "mView");
  modelProjectionLocation  = glGetUniformLocation(program, "mProj");
  modelInverseTransposeLocation = glGetUniformLocation(program, "mModel_InverseTranpose");

  useOverrideColourLocation = glGetUniformLocation(program, "bUseOverrideColour");
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
