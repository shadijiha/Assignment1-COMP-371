#pragma once
#include <memory>
#include <glm/vec3.hpp>
#include <glm/vec4.hpp>

#include "Camera.h"
#include "Light.h"
#include "Shader.h"
#include "util/Texture.h"
#include "util/VertexArray.h"


class Sphere {
public:
	Sphere(const glm::vec3& position = {0, 0, 0},
		const glm::vec3& rotation = {0, 0, 0},
		const glm::vec3& scale = {1, 1, 1},
		const glm::vec4& color = { 1, 1, 1, 1 });

	void onDraw(float dt);

	void setShader(std::shared_ptr<Shader> shader) { this->shader = shader; }
	void setCamera(std::shared_ptr<Camera> cam) { this->camera = cam; }
	void setRotationOrigin(const glm::vec3& rotaionOrigin) {
		this->rotationOrigin = rotaionOrigin;
		this->rotationOriginPtr = &this->rotationOrigin;
	}

	void setLight(const std::shared_ptr<Light>& light) { this->light = light; }
	void setTexture(const std::shared_ptr<Texture>& texture) { this->texture = texture; }

private:
	static int init();

public:
	glm::vec3 position;
	glm::vec3 rotation;
	glm::vec3 scale;
	glm::vec4 color;

private:
	inline static std::shared_ptr<VertexArray> vao = nullptr;
	std::shared_ptr<Shader> shader = nullptr;
	std::shared_ptr<Camera> camera = nullptr;
	std::shared_ptr<Light> light = nullptr;
	std::shared_ptr<Texture> texture = nullptr;

	glm::vec3 rotationOrigin;
	glm::vec3* rotationOriginPtr;
};
