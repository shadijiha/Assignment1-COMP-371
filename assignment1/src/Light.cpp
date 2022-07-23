#include "Light.h"

#include "Renderer.h"

glm::mat4 Light::getProjection() {
	float near_plane = 0.01f, far_plane = 100.0f;
	return glm::ortho(-10.0f, 10.0f, -10.0f, 10.0f, near_plane, far_plane);

	//return glm::perspective(glm::radians(90.0f),
	//	(float)Renderer::getInfo().shadow_width / (float)Renderer::getInfo().shadow_height, 3.0f, 10.0f);
}
