#pragma once
#include <glm/glm.hpp>
#include <glm/gtx/transform.hpp>


struct TransformBuilder {
	TransformBuilder()	{}

	inline static TransformBuilder create() {
		return TransformBuilder();
	}

	inline void translate(const glm::vec3& pos) {
		transform *= glm::translate(glm::mat4(1.0), pos);
	}

	inline void rotate(const glm::vec3& rot, const glm::vec3& around = glm::vec3(0)) {
		transform *= (glm::translate(glm::mat4(1.0), around)
		 * glm::rotate(glm::mat4(1), rot.x, {1, 0, 0})
		 * glm::rotate(glm::mat4(1), rot.y, { 0, 1, 0 })
		 * glm::rotate(glm::mat4(1), rot.z, { 0, 0, 1 })
		 * glm::translate(glm::mat4(1.0), -around));
	}

	inline void scale(const glm::vec3& scale) {
		transform *= glm::scale(glm::mat4(1), scale);
	}

	inline const glm::mat4& get() const {
		return transform;
	}

private:
	glm::mat4 transform = glm::mat4(1.0f);
};
