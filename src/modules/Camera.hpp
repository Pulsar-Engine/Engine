#ifndef CAMERA_HPP_
    #define CAMERA_HPP_

    #include <glm/glm.hpp>

enum Direction {
    LEFT,
    RIGHT,
    FORWARD,
    BACKWARD,
    UP,
    DOWN
};

class Camera {
    public:
        Camera(uint32_t width, uint32_t height);
        ~Camera() = default;
        void move(Direction direction, float deltaTime);
        void changeDirection(double xPos, double yPos);
        glm::vec3 &getPos();
        glm::vec3 &getFront();
        glm::vec3 &getUp();
        float getFOV();
    protected:
    private:
        glm::vec3 _pos;
        glm::vec3 _front;
        glm::vec3 _up;
        float _yaw;
        float _pitch;
        float _fov;
        double _lastX;
        double _lastY;
        double _sensitivity;
        float _speed;
        bool _firstDirection;
};

#endif /* !CAMERA_HPP_ */
