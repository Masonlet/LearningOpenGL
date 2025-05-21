#include "scene.hpp"
#include "generator.hpp"
#include "parser.hpp"
#include <cstdio>

class DefaultDemo : public Scene {
	bool loaded{false};
	unsigned int count{1};

public:
	void Load(Model* models) override {
		if (!models[0].mesh)
			models[0].mesh = new Mesh();
		if (!createTriangle(*models[0].mesh)) {
			fprintf(stderr, "Failed to create DefaultDemo triangle");
			loaded = false;
		}

		loaded = true;
	}
	
	inline bool isLoaded() const { return loaded; }
	unsigned int objectCount() const { return count; }
};

class SpaceDemo : public Scene {
	bool loaded{false};
	const unsigned int count{10};

public:
	void Load(Model* models) override {
		const char* path = "Assets/Models/GalacticaOriginal_ASCII_no_text.ply";
		constexpr float spacing{100.0f};
		constexpr bool hasNormals{false};

		if (!createModelGrid(models, path, 0, count, spacing, hasNormals)) {
			fprintf(stderr, "Failed to create SpaceDemo modelGrid");
			loaded = false;
		}

		loaded = true;
	}

	inline bool isLoaded() const { return loaded; }
	inline unsigned int objectCount() const { return count; }
};

class PlaneDemo : public Scene {
	bool loaded{false};
	const unsigned int count{2500};

public:
	void Load(Model* models) override {
		constexpr float spacing{1.0f};

		if (!createSquareGrid(models, 0, count, spacing, {90.0f, 0.0f, 0.0f})) {
			fprintf(stderr, "Failed to create PlaneDemo squareGrid");
			loaded = false;
		}

		loaded = true;
	}

	inline bool isLoaded() const { return loaded; }
	inline unsigned int objectCount() const { return count; }
};

Scene* createSceneFromName(const char* name) {
	if (strEquals(name, "Default")) return new DefaultDemo();
	else if (strEquals(name, "Space")) return new SpaceDemo();
	else if (strEquals(name, "Plane")) return new PlaneDemo();

	return nullptr;
}