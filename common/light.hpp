#pragma once
#include <glm/glm.hpp>

class Light {
public:
    Light(
        const glm::vec3& position = glm::vec3(2.0f, 2.0f, 2.0f),
        const glm::vec3& color = glm::vec3(1.0f),
        const glm::vec3& direction = glm::vec3(0.0f, -1.0f, 0.0f),
        float cutOff = glm::cos(glm::radians(12.5f)),
        float outerCutOff = glm::cos(glm::radians(17.5f))
    );

    
    glm::vec3 getPosition() const;
    void setPosition(const glm::vec3& position);
    glm::vec3 getColor() const;
    void setColor(const glm::vec3& color);

   
    glm::vec3 getDirection() const;
    void setDirection(const glm::vec3& direction);
    float getCutOff() const;
    void setCutOff(float cutOff);
    float getOuterCutOff() const;
    void setOuterCutOff(float outerCutOff);

private:
    glm::vec3 position;
    glm::vec3 color;

   
    glm::vec3 direction;
    float cutOff;
    float outerCutOff;
};