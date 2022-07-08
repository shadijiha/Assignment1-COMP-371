#include "Camera.h"

#include <iostream>
#include <glm/gtx/transform.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "KeyCodes.h"
#include "SceneManager.h"

Camera::Camera(const SceneManager& manager, uint32_t width, uint32_t height)
    : manager(manager), width(width), height(height)
{
    //glViewport(0, 0, width, height);
    recalculateMatrix();
}
void Camera::setWindowSize(uint32_t width, uint32_t height)
{
    this->width = width;
    this->height = height;
}

void Camera::onUpdate(float dt) {

    if (manager.isMouseButtonDown(MouseCode::BUTTON_LEFT) && zoomEnabled) {
        const glm::vec2& mouse = manager.getMousePos();
        glm::vec2 delta = (mouse - m_InitialMousePosition) * 0.03f;
        m_InitialMousePosition = mouse;

        position.z += delta.y;
        recalculateMatrix();
    }

    if (manager.isMouseButtonDown(MouseCode::BUTTON_RIGHT) && zoomEnabled) {
        const glm::vec2& mouse = manager.getMousePos();
        glm::vec2 delta = (mouse - m_InitialMousePosition) * 0.03f;
        m_InitialMousePosition = mouse;

        position.x += delta.x;
        recalculateMatrix();
    }

    if (manager.isMouseButtonDown(MouseCode::BUTTON_MIDDLE) && zoomEnabled) {
        const glm::vec2& mouse = manager.getMousePos();
        glm::vec2 delta = (mouse - m_InitialMousePosition) * 0.03f;
        m_InitialMousePosition = mouse;

        position.y += delta.y;
        recalculateMatrix();
    }
}

void Camera::recalculateMatrix()
{
    glm::mat4 view = glm::lookAt(position, { position.x, position.y, 0 }, glm::vec3(0.0f, 1.0f, 0.0f));
    view = glm::translate(view, position);
    view = glm::rotate(view, glm::radians(rotation.x), { 1, 0, 0 });
    view = glm::rotate(view, glm::radians(rotation.y), { 0, 1, 0 });
    view = glm::rotate(view, glm::radians(rotation.z), { 0, 0, 1 });
    view = glm::translate(view, -position);
    glm::mat4 projectionMatrix = glm::perspective(glm::radians(90.0f), (float)width / (float)height, 0.1f, 1000.0f);

    this->view = view;
    this->proj = projectionMatrix;

    viewProj = projectionMatrix * view;
}
