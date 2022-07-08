#pragma once
#include <glm/glm.hpp>

class SceneManager;

class Camera {

public:
	Camera(const SceneManager& manager, uint32_t width, uint32_t height);

	void setPosition(const glm::vec3& pos) { position = pos; recalculateMatrix(); }
	void setRotation(const glm::vec3& rot) { rotation = rot; recalculateMatrix(); }
	void setWindowSize(uint32_t width, uint32_t height);


	void onUpdate(float dt);

	const glm::mat4& getViewProjection() const { return viewProj;}

	inline const glm::vec3& getPosition() const { return position; }
	inline const glm::vec3& getRotation() const { return rotation; }
	inline const uint32_t getWidth() const { return width; }
	inline const uint32_t getHeight() const { return height; }
private:
	void recalculateMatrix();

	const SceneManager& manager;
	glm::vec3 position = glm::vec3(0.0f);
	glm::vec3 rotation = glm::vec3(0.0f);

	glm::mat4 viewProj = glm::mat4(1.0f);

	uint32_t width, height;

	glm::vec2 m_InitialMousePosition = glm::vec2(0);

	friend class SceneManager;
};
