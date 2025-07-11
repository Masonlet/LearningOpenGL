#include "math/geometry.hpp"

Triangle::Triangle() : position{ 0.0f, 0.0f, 0.0f, 0.0f },
	rotation{ 0.0f, 0.0f, 0.0f },
	scale{ 1.0f, 1.0f, 1.0f } { }

Square::Square() : position{ 0.0f, 0.0f, 0.0f, 0.0f },
	rotation{ 0.0f, 0.0f, 0.0f },
	size{ 1.0f, 1.0f } { }

Grid::Grid() : count{ 1 },
	spacing{ 1.0f },
	start{ 0.0f, 0.0f, 0.0f },
	rotation{ 0.0f, 0.0f, 0.0f },
	scale{ 1.0f, 1.0f, 1.0f } { }