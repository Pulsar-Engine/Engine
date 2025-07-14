#include "Camera.hpp"

Camera::Camera(uint32_t width, uint32_t height) :
    _pos(0.0f, -3.0f, 1.5f),
    _front(0.0f, 1.0f, -0.5f),
    _up(0.0f, 0.0f, 1.0f),
    _yaw(glm::degrees(atan2(_front.y, _front.x))),
    _pitch(glm::degrees(asin(_front.z))),
    _fov(45.0f),
    _lastX(width / 2.0f),
    _lastY(height / 2.0f),
    _sensitivity(0.1f),
    _speed(2.5f),
    _firstDirection(true)
{

}

void Camera::changeDirection(double xPos, double yPos) {
    if (_firstDirection) {
        _lastX = xPos;
        _lastY = yPos;
        _firstDirection = false;
        return;
    }
    double xOffset = _lastX - xPos;
    double yOffset = _lastY - yPos;
    _lastX = xPos;
    _lastY = yPos;
    xOffset *= _sensitivity;
    yOffset *= _sensitivity;
    _yaw += static_cast<float>(xOffset);
    _pitch += static_cast<float>(yOffset);
    if (_pitch > 89.0f) _pitch = 89.0f;
    if (_pitch < -89.0f) _pitch = -89.0f;
    glm::vec3 direction;
    direction.x = cos(glm::radians(_yaw)) * cos(glm::radians(_pitch));
    direction.y = sin(glm::radians(_yaw)) * cos(glm::radians(_pitch));
    direction.z = sin(glm::radians(_pitch));
    _front = glm::normalize(direction);
}

void Camera::move(Direction direction, float deltaTime) {
    float sign = 1;
    switch (direction) {
        case DOWN:
            sign = -1;
            [[fallthrough]];
        case UP:
            _pos += _up * _speed * deltaTime * sign;
            break;
        case LEFT:
            sign = -1;
            [[fallthrough]];
        case RIGHT:
            _pos += glm::normalize(glm::cross(_front, _up)) * _speed * deltaTime * sign;
            break;
        case BACKWARD:
            sign = -1;
            [[fallthrough]];
        case FORWARD:
            _pos += _speed * _front * deltaTime * sign;
            break;
    }
}

glm::vec3 &Camera::getPos() {
    return _pos;
}

glm::vec3 &Camera::getFront() {
    return _front;
}

glm::vec3 &Camera::getUp() {
    return _up;
}


float Camera::getFOV() {
    return _fov;
}