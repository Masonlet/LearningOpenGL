#pragma once

#include <cstddef>

class InputManager;

template <typename T>
struct Controller {
	virtual ~Controller() = default;

	std::size_t current{ 0 };
	inline void increment(std::size_t maxCount) { if ((maxCount - 1) > current) current++; }
	inline void decrement() { if (current != 0) current--; }

	virtual void update(T& data, const InputManager& input, float deltaTime) = 0;
};