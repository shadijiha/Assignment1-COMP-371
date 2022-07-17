#pragma once
#include <glm/vec3.hpp>
#include <glm/vec4.hpp>
#include <glm/ext/matrix_clip_space.hpp>
#include <glm/ext/matrix_transform.hpp>


struct Light {
	glm::vec3 position = {1.7, 1.8, -2};
	glm::vec4 color = { 1, 1, 1, 1 };
	float ambientStrength = 0.5;

	// These functions are used for shadows
	inline glm::mat4 getProjection() {
		float near_plane = 1.0f, far_plane = 7.5f;
		return glm::ortho(-10.0f, 10.0f, -10.0f, 10.0f, near_plane, far_plane);
	}

	inline glm::mat4 getView() {
		return glm::lookAt(position,
			glm::vec3(0.0f, 0.0f, 0.0f),
			glm::vec3(0.0f, 1.0f, 0.0f));
	}

	inline glm::mat4 getSpaceMatrix() {
		return getProjection() * getView();
	}
};
