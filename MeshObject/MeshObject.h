#pragma once
#include <string>
#include <glm/glm.hpp>
#include <glm/vec3.hpp>

class MeshObject
{

public:
	MeshObject();
	std::string meshName;
	std::string path;

	std::string friendlyName;

	glm::vec3 position;
	glm::vec3 rotation;
	float scale;
	bool isWireframe;

	glm::vec4 RGBA_color;

	bool bUse_RGBA_color;
	bool bDoNotLight;

};

