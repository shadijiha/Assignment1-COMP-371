#pragma once
#include <glm/glm.hpp>

class Camera {

public:
	Camera(uint32_t width, uint32_t height);

	void setPosition(const glm::vec3& pos) { position = pos; recalculateMatrix(); }
	void setRotation(const glm::vec3& rot) { rotation = rot; recalculateMatrix(); }
	void setWindowSize(uint32_t width, uint32_t height);

	const glm::mat4& getViewProjection() const { return viewProj;}

	inline const glm::vec3& getPosition() const { return position; }
	inline const glm::vec3& getRotation() const { return rotation; }
	inline const uint32_t getWidth() const { return width; }
	inline const uint32_t getHeight() const { return height; }
private:
	void recalculateMatrix();

	glm::vec3 position = glm::vec3(0.0f);
	glm::vec3 rotation = glm::vec3(0.0f);

	glm::mat4 viewProj;

	uint32_t width, height;
};