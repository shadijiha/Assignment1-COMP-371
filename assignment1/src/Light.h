#pragma once
#include <glm/vec3.hpp>
#include <glm/vec4.hpp>

struct Light {
	glm::vec3 position = {0, 30, 0};
	glm::vec4 color = { 1, 1, 1, 1 };
	float ambientStrength = 0.5;
};
