#include "Camera.h"
#include <glm/gtx/transform.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <GL/glew.h>

Camera::Camera(uint32_t width, uint32_t height)
    : width(width), height(height)
{
    //glViewport(0, 0, width, height);
}
void Camera::setWindowSize(uint32_t width, uint32_t height)
{
    this->width = width;
    this->height = height;
}
glm::mat4 Camera::getViewProjection()
{
    glm::mat4 view = glm::lookAt(position, { position.x, position.y, 0 }, glm::vec3(0.0f, 1.0f, 0.0f));
    view = glm::rotate(view, glm::radians(rotation.x), {1, 0, 0});
    view = glm::rotate(view, glm::radians(rotation.y), { 0, 1, 0 });
    view = glm::rotate(view, glm::radians(rotation.z), { 0, 0, 1 });
    glm::mat4 projectionMatrix = glm::perspective(glm::radians(90.0f), (float)width / (float)height, 0.1f, 100.0f);

	return projectionMatrix * view;
}
