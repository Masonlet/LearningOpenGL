#pragma once

#include <cstdint>

class Shader {
public:
  uint32_t programID{ 0 };
  uint32_t vertexID{ 0 };
  uint32_t fragmentID{ 0 };
  bool     linked{ false };

  inline bool empty() const { return programID == 0 || !linked; }

  Shader() = default;
  ~Shader() = default;                 

  Shader(const Shader&) = delete;
  Shader& operator=(const Shader&) = delete;

  Shader(Shader&& other) noexcept { *this = static_cast<Shader&&>(other); }
  Shader& operator=(Shader&& other) noexcept {
    if (this != &other) {
      programID = other.programID;
      vertexID = other.vertexID;
      fragmentID = other.fragmentID;
      linked = other.linked;

      other.programID = 0;
      other.vertexID = 0;
      other.fragmentID = 0;
      other.linked = false;
    }
    return *this;
  }
};