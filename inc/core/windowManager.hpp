#pragma once
  
#include "window.hpp"

class WindowManager {
public:
  WindowManager();
  ~WindowManager();

  bool createWindow(const unsigned int width, const unsigned int height, const char* title);
  void destroyWindow();

  inline Window *getWindow() const { return window; }

private:
  Window* window = nullptr;
};