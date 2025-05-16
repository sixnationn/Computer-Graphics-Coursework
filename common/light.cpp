#include "light.hpp"

Light::Light(const glm::vec3& position, const glm::vec3& color, const glm::vec3& direction, float cutOff, float outerCutOff)
    : position(position), color(color), direction(direction), cutOff(cutOff), outerCutOff(outerCutOff) {}

glm::vec3 Light::getPosition() const { return position; }
void Light::setPosition(const glm::vec3& pos) { position = pos; }

glm::vec3 Light::getColor() const { return color; }
void Light::setColor(const glm::vec3& col) { color = col; }

glm::vec3 Light::getDirection() const { return direction; }
void Light::setDirection(const glm::vec3& dir) { direction = dir; }

float Light::getCutOff() const { return cutOff; }
void Light::setCutOff(float co) { cutOff = co; }

float Light::getOuterCutOff() const { return outerCutOff; }
void Light::setOuterCutOff(float oco) { outerCutOff = oco; }
