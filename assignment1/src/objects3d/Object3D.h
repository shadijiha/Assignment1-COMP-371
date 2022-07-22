#pragma once
#include <util/VertexArray.h>
#include <Shader.h>
#include <Camera.h>
#include <Light.h>
#include <util/Texture.h>
#include <Renderer.h>

class Object3D
{
public:
	Object3D(const std::string& path);

	void onDraw(float dt);

	static void setShader(std::shared_ptr<Shader> shader) { Object3D::shader = shader; }
	void setCamera(std::shared_ptr<Camera> cam) { this->camera = cam; }
	void setLight(const std::shared_ptr<Light>& light) { this->light = light; }
	void setTexture(const std::shared_ptr<Texture>& texture) { this->texture = texture; }

	void setPosition(const glm::vec3& pos) {
		position = pos + glm::vec3{ -2.6, -1.3, -10.8 };
	}

public:
	glm::vec3 position = { -2.6, -1.3, -10.8 }; // For some reason blender is acting weird and I need this
	glm::vec3 rotation = {0, 0, 0};
	glm::vec3 scale = {1, 1, 1};
	glm::vec4 color = {1, 1, 1, 1};
	glm::vec3 rotationOrigin = {0, 0, 0};

private:
	

	std::shared_ptr<VertexArray> vao;
	inline static std::shared_ptr<Shader> shader = Renderer::shader;
	std::shared_ptr<Camera> camera = Renderer::camera;
	std::shared_ptr<Light> light = Renderer::light;
	std::shared_ptr<Texture> texture = nullptr;
};

