#pragma once
#include <memory>
#include <glm/gtx/transform.hpp>

#include "Shader.h"

enum class RenderingMode { Triangles, Lines, Points };

class GameObject {
public:
	virtual void onCreate()			{}
	virtual void onUpdate(float dt) {}
	virtual void onDestroyed()		{}

	void setPosition(const glm::vec3& pos)						{ position = pos; }
	void setScale(const glm::vec3& scale)						{ this->scale = scale; }
	void setRotation(const glm::vec3& rotation)					{ this->rotation = rotation; }
	void setRotationOrigin(const glm::vec3& pos)				{ rotationOrigin = pos; }
	void setColor(const glm::vec4& color)						{ this->color = color; }
	void setRenderingMode(RenderingMode mode)					{ this->mode = mode; }
	void setParent(std::shared_ptr<GameObject> parent)			{ this->parent = parent; }

	const glm::vec3& getPosition()			const { return position; }
	const glm::vec3& getScale()				const { return scale; }
	const glm::vec3& getRotation()			const { return rotation; }
	const glm::vec3& getRotationOrigin()	const { return rotationOrigin; }
	const glm::vec4& getColor()				const { return   color; }
	const std::shared_ptr<GameObject>& getParent()	const { return parent; }


	glm::mat4 getTransform() const {
		auto scale = parent == nullptr ? this->scale :  this->scale * parent->scale;
		auto position = parent == nullptr ? this->position : this->position + parent->position;
		auto rotation = parent == nullptr ? this->rotation : this->rotation + parent->rotation;
		auto rotationOrigin = parent == nullptr ? this->rotationOrigin : this->rotationOrigin + parent->rotationOrigin;
		return glm::translate(glm::mat4(1.0f), rotationOrigin)
			* glm::rotate(glm::mat4(1.0), glm::radians(rotation.x), { 1, 0, 0 })
			* glm::rotate(glm::mat4(1.0), glm::radians(rotation.y), { 0, 1, 0 })
			* glm::rotate(glm::mat4(1.0), glm::radians(rotation.z), { 0, 0, 1 })
			* glm::translate(glm::mat4(1.0f), -rotationOrigin)
			* glm::translate(glm::mat4(1.0), position)
			* glm::scale(scale);
	}
protected:
	std::shared_ptr<GameObject> parent = nullptr;

	glm::vec3 position = glm::vec3(0.0f);
	glm::vec3 scale = glm::vec3(1.0f);
	glm::vec3 rotation = glm::vec3(0.0f);
	glm::vec3 rotationOrigin = glm::vec3(0.0f);
	glm::vec4 color = glm::vec4(1.0f);
	RenderingMode mode = RenderingMode::Triangles;


};
