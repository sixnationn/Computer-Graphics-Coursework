#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

class Camera {
public:
    // Constructor with initial position
    Camera(glm::vec3 position = glm::vec3(0.0f, 0.0f, 5.0f),
        glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f),
        float yaw = -90.0f, float pitch = 0.0f);

    // Returns the view matrix calculated using Euler Angles and the LookAt Matrix
    glm::mat4 getViewMatrix();

    // Process keyboard input (WASD movement)
    void ProcessKeyboard(GLFWwindow* window, float deltaTime);

    // Process mouse movement for camera rotation
    void ProcessMouseMovement(float xoffset, float yoffset, GLboolean constrainPitch = true);

    // Process mouse scroll for zoom
    void ProcessMouseScroll(float yoffset);

    // Get the current zoom value
    float getZoom() const { return Zoom; }

    // Get the current position of the camera
    glm::vec3 getPosition() const { return Position; }

    // Set the current position of the camera
    void setPosition(const glm::vec3& position); // Added setter method

private:
    // Camera attributes
    glm::vec3 Position;
    glm::vec3 Front;
    glm::vec3 Up;
    glm::vec3 Right;
    glm::vec3 WorldUp;

    // Euler Angles
    float Yaw;
    float Pitch;

    // Camera options
    float MovementSpeed;
    float MouseSensitivity;
    float Zoom;

    // Update camera vectors based on Euler Angles
    void updateCameraVectors();
};