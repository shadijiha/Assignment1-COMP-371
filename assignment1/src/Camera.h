#pragma once
#include <glm/glm.hpp>

class Camera {

public:
	Camera(uint32_t width, uint32_t height);

	void setPosition(const glm::vec3& pos) { position = pos; }
	void setRotation(const glm::vec3& rot) { rotation = rot; }

	glm::mat4 getViewProjection();

	inline const glm::vec3& getPosition() const { return position; }

private:
	glm::vec3 position = glm::vec3(0.0f);
	glm::vec3 rotation = glm::vec3(0.0f);

	uint32_t width, height;
};