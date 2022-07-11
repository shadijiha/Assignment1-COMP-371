#pragma once
#include <glm/vec3.hpp>
#include <glm/vec4.hpp>

#include "GameObject.h"
#include "Shader.h"
#include "util/VertexArray.h"


class Cube : public GameObject {
public:
	Cube();
	Cube(const glm::vec3& pos, const glm::vec3& rotation, const glm::vec3& scale, const glm::vec4& color = glm::vec4(1.0f));
	virtual ~Cube()	{}

	// Render
	virtual void onUpdate(float dt) override;
	virtual void onCreate() override	{}
	virtual void onDestroyed() override {}

	void setShader(std::shared_ptr<Shader> shader) { this->shader = shader; }

	static void initVAO();
private:
	std::shared_ptr<VertexArray> vao;
	std::shared_ptr<Shader> shader;

	inline static std::shared_ptr<VertexArray> GlobalVAO = nullptr;
	inline static std::shared_ptr<VertexArray> GlobalShader = nullptr;
};
