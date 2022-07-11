#pragma once
#include <memory>
#include <glm/gtx/transform.hpp>

#include "Shader.h"

enum class RenderingMode { Triangles, Lines, Points, LineLoop, Default };

class GameObject {
public:
	GameObject() = default;
	virtual ~GameObject() = default;

	virtual void onCreate()			{}
	virtual void onUpdate(float dt) {}
	virtual void onDestroyed()		{}

	void setParent(std::shared_ptr<GameObject> parent)			{ this->parent = parent; }
	const std::shared_ptr<GameObject>& getParent()	const { return parent; }


	glm::mat4 getTransform() const {
		glm::vec3 scale = this->scale *  this->scaleFactor;
		glm::vec3 position = this->position;
		glm::vec3 rotation = this->rotation;
		glm::vec3 rotationOrigin = this->rotationOrigin;

		/*if (parent) {
			scale = this->scale * this->scaleFactor * parent->scaleFactor;
			position = this->position + parent->position;
			rotation = this->rotation + parent->rotation;
			rotationOrigin = this->rotationOrigin + parent->rotationOrigin;
		}		*/
		
		auto transform = glm::translate(glm::mat4(1.0f), rotationOrigin)
			* glm::rotate(glm::mat4(1.0), glm::radians(rotation.x), { 1, 0, 0 })
			* glm::rotate(glm::mat4(1.0), glm::radians(rotation.y), { 0, 1, 0 })
			* glm::rotate(glm::mat4(1.0), glm::radians(rotation.z), { 0, 0, 1 })
			* glm::translate(glm::mat4(1.0f), -rotationOrigin)
			* glm::translate(glm::mat4(1.0), position)
			* glm::scale(scale);

		return parent == nullptr ? transform :  parent->getTransform() * transform;
	}

public:
	glm::vec3 position = glm::vec3(0.0f);
	glm::vec3 scale = glm::vec3(1.0f);
	glm::vec3 scaleFactor = glm::vec3(1.0f);
	glm::vec3 rotation = glm::vec3(0.0f);
	glm::vec3 rotationOrigin = glm::vec3(0.0f);
	glm::vec4 color = glm::vec4(1.0f);
	RenderingMode renderingMode = RenderingMode::Default;

protected:
	std::shared_ptr<GameObject> parent = nullptr;	
};

