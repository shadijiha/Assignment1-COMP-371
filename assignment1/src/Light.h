#pragma once
#include <glm/vec3.hpp>
#include <glm/vec4.hpp>
#include <glm/ext/matrix_clip_space.hpp>
#include <glm/ext/matrix_transform.hpp>


struct Light {
	glm::vec3 position = {1.7, 1.8, -2};
	glm::vec4 C_d = glm::vec4(1.0);
	glm::vec4 C_a = glm::vec4(1.0);
	glm::vec4 C_l = glm::vec4(1.0);

	// Composante de Phong
	float k_d = 0.5;
	float k_a = 0.5;
	float k_s = 0.5;
	float s = 0.5;

	// These functions are used for shadows
	glm::mat4 getProjection();

	inline glm::mat4 getView() {
		return glm::lookAt(position,
			glm::vec3(0.0f, 0.0f, 0.0f),
			glm::vec3(0.0f, 1.0f, 0.0f));
	}

	inline glm::mat4 getSpaceMatrix() {
		return getProjection() * getView();
	}
};


