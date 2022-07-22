#include "Light.h"

#include "Renderer.h"

glm::mat4 Light::getProjection() {
	float near_plane = 1.0f, far_plane = 10.0f;
	return glm::ortho(-10.0f, 10.0f, -10.0f, 10.0f, near_plane, far_plane);

	//return glm::perspective(glm::radians(90.0f),
	//	(float)Renderer::getInfo().shadow_width / (float)Renderer::getInfo().shadow_height, 0.1f, 10.0f);
}
