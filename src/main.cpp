#include "engine.hpp"

int main() {
	Engine engine;

	const ModelInit models[] = {
		// Rainbow 
		{ "Assets/Models/GalacticaOriginal_ASCII_no_text.ply", 
		Mat4::translation(Vec3{0.0f, 0.0f, 100.0f}) },

		// Green
		{ "Assets/Models/de--lorean.ply",                      
		Mat4::translation(Vec3{0.0f, 0.0f, 10.0f}) },

		// Blue
		{ "Assets/Models/dolphin.ply",                         
		Mat4::translation(Vec3{0.0f, -100.0f, -50.0f}) },

		//Yellow
		{ "Assets/Models/AUDI_TT_COUPE.ply",                   
		Mat4::translation(Vec3{-20.0f, 0.0f, 10.0f}) },

		//Random
		{ "Assets/Models/homer.ply",                           
		Mat4::translation(Vec3{-20.0f, 0.0f, 50.0f}) * Mat4::scale(Vec3{100.0f, 100.0f, 100.0f}) }
	};

	for (const ModelInit& model : models) {
		engine.loadModel(model.path, model.transform);
	}

	engine.run();
}