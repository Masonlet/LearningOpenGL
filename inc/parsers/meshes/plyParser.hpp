#pragma once

struct Mesh;

bool parsePlyMesh(const std::string& path, Mesh& drawInfo);
