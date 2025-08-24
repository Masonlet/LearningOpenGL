#pragma once

#include <string>

enum class ObjectKind { Unknown, Model, Camera, Light, Maze };

class Object {
	public:
		Object(const std::string& name) : name(name) {}
		virtual ~Object() = default;

		const std::string& getName() const { return name; }
		virtual ObjectKind kind() const { return ObjectKind::Unknown; }

private:
	  std::string name;
};