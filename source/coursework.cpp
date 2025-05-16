#include <iostream>
#include <cmath>

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <common/shader.hpp>
#include <common/texture.hpp>
#include <common/maths.hpp>
#include <common/camera.hpp>
#include <common/model.hpp>
#include <common/light.hpp>

void keyboardInput(GLFWwindow* window);
void mouseCallback(GLFWwindow* window, double xpos, double ypos);
void scrollCallback(GLFWwindow* window, double xoffset, double yoffset);
void checkCollisions(Camera& camera);

Camera camera(glm::vec3(0.0f, 0.0f, 7.0f)); 
float lastX = 1024.0f / 2.0f;
float lastY = 768.0f / 2.0f;
bool firstMouse = true;

const float ROOM_WIDTH = 10.0f;
const float ROOM_DEPTH = 10.0f;
const float ROOM_HEIGHT = 5.0f;

Light light(glm::vec3(2.0f, 3.0f, 2.0f)); 
Light spotLight(
    glm::vec3(0.0f, 3.5f, 0.0f),           
    glm::vec3(1.0f, 1.0f, 0.0f),           
    glm::vec3(0.0f, -1.0f, 0.0f),          
    glm::cos(glm::radians(12.5f)),         
    glm::cos(glm::radians(17.5f))          
);

const float LIGHT_SPEED = 2.0f;

int main()
{
    if (!glfwInit())
    {
        std::cerr << "Failed to initialize GLFW\n";
        return -1;
    }

    glfwWindowHint(GLFW_SAMPLES, 4);
    glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    GLFWwindow* window = glfwCreateWindow(1024, 768, "CG Coursework", NULL, NULL);
    if (!window)
    {
        std::cerr << "Failed to create GLFW window\n";
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);

    glewExperimental = true;
    if (glewInit() != GLEW_OK)
    {
        std::cerr << "Failed to initialize GLEW\n";
        glfwTerminate();
        return -1;
    }

    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);
    glDisable(GL_CULL_FACE); 

    glfwSetInputMode(window, GLFW_STICKY_KEYS, GL_TRUE);
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    glfwSetCursorPosCallback(window, mouseCallback);
    glfwSetScrollCallback(window, scrollCallback);

    const float cubeVertices[] = {
        //pos                //normals
        -1,-1, 1,            0, 0, 1,
         1,-1, 1,            0, 0, 1,
         1, 1, 1,            0, 0, 1,
        -1, 1, 1,            0, 0, 1,

         1,-1, 1,            1, 0, 0,
         1,-1,-1,            1, 0, 0,
         1, 1,-1,            1, 0, 0,
         1, 1, 1,            1, 0, 0,

         1,-1,-1,            0, 0,-1,
        -1,-1,-1,            0, 0,-1,
        -1, 1,-1,            0, 0,-1,
         1, 1,-1,            0, 0,-1,

        -1,-1,-1,           -1, 0, 0,
        -1,-1, 1,           -1, 0, 0,
        -1, 1, 1,           -1, 0, 0,
        -1, 1,-1,           -1, 0, 0,

        -1,-1,-1,            0,-1, 0,
         1,-1,-1,            0,-1, 0,
         1,-1, 1,            0,-1, 0,
        -1,-1, 1,            0,-1, 0,

        -1, 1, 1,             0, 1, 0,
         1, 1, 1,             0, 1, 0,
         1, 1,-1,             0, 1, 0,
        -1, 1,-1,             0, 1, 0,
    };

    const float cubeUVs[] = {
        0,0,  1,0,  1,1,  0,1,
        0,0,  1,0,  1,1,  0,1,
        0,0,  1,0,  1,1,  0,1,
        0,0,  1,0,  1,1,  0,1,
        0,0,  1,0,  1,1,  0,1,
        0,0,  1,0,  1,1,  0,1
    };

    const unsigned int cubeIndices[] = {
         0, 1, 2,   2, 3, 0,
         4, 5, 6,   6, 7, 4,
         8, 9,10,  10,11, 8,
        12,13,14,  14,15,12,
        16,17,18,  18,19,16,
        20,21,22,  22,23,20
    };

    const float roomVertices[] = {
        //floor
        -ROOM_WIDTH, -ROOM_HEIGHT, -ROOM_DEPTH,  0, 1, 0,
         ROOM_WIDTH, -ROOM_HEIGHT, -ROOM_DEPTH,  0, 1, 0,
         ROOM_WIDTH, -ROOM_HEIGHT,  ROOM_DEPTH,  0, 1, 0,
        -ROOM_WIDTH, -ROOM_HEIGHT,  ROOM_DEPTH,  0, 1, 0,

        //ceiling
        -ROOM_WIDTH, ROOM_HEIGHT, -ROOM_DEPTH,   0,-1, 0,
         ROOM_WIDTH, ROOM_HEIGHT, -ROOM_DEPTH,   0,-1, 0,
         ROOM_WIDTH, ROOM_HEIGHT,  ROOM_DEPTH,   0,-1, 0,
        -ROOM_WIDTH, ROOM_HEIGHT,  ROOM_DEPTH,   0,-1, 0,

        //front
        -ROOM_WIDTH, -ROOM_HEIGHT, -ROOM_DEPTH,  0, 0, 1,
         ROOM_WIDTH, -ROOM_HEIGHT, -ROOM_DEPTH,  0, 0, 1,
         ROOM_WIDTH, ROOM_HEIGHT, -ROOM_DEPTH,    0, 0, 1,
        -ROOM_WIDTH, ROOM_HEIGHT, -ROOM_DEPTH,    0, 0, 1,

        //right
         ROOM_WIDTH, -ROOM_HEIGHT, -ROOM_DEPTH, -1, 0, 0,
         ROOM_WIDTH, -ROOM_HEIGHT,  ROOM_DEPTH, -1, 0, 0,
         ROOM_WIDTH, ROOM_HEIGHT,  ROOM_DEPTH,  -1, 0, 0,
         ROOM_WIDTH, ROOM_HEIGHT, -ROOM_DEPTH,  -1, 0, 0,

         //back
          ROOM_WIDTH, -ROOM_HEIGHT,  ROOM_DEPTH,  0, 0,-1,
         -ROOM_WIDTH, -ROOM_HEIGHT,  ROOM_DEPTH,  0, 0,-1,
         -ROOM_WIDTH, ROOM_HEIGHT,  ROOM_DEPTH,    0, 0,-1,
          ROOM_WIDTH, ROOM_HEIGHT,  ROOM_DEPTH,    0, 0,-1,

          //left
          -ROOM_WIDTH, -ROOM_HEIGHT,  ROOM_DEPTH,  1, 0, 0,
          -ROOM_WIDTH, -ROOM_HEIGHT, -ROOM_DEPTH,  1, 0, 0,
          -ROOM_WIDTH, ROOM_HEIGHT, -ROOM_DEPTH,   1, 0, 0,
          -ROOM_WIDTH, ROOM_HEIGHT,  ROOM_DEPTH,   1, 0, 0,
    };

    const float roomUVs[] = {
        0,0, 5,0, 5,5, 0,5,
        0,0, 5,0, 5,5, 0,5,
        0,0, 5,0, 5,5, 0,5,
        0,0, 5,0, 5,5, 0,5,
        0,0, 5,0, 5,5, 0,5,
        0,0, 5,0, 5,5, 0,5,
    };

    const unsigned int roomIndices[] = {
        0,1,2, 2,3,0,
        4,5,6, 6,7,4,
        8,9,10, 10,11,8,
        12,13,14, 14,15,12,
        16,17,18, 18,19,16,
        20,21,22, 22,23,20,
    };

    //buffer setups
    unsigned int cubeVAO, cubeVBO, cubeUVVBO, cubeEBO;
    glGenVertexArrays(1, &cubeVAO);
    glBindVertexArray(cubeVAO);

    glGenBuffers(1, &cubeVBO);
    glBindBuffer(GL_ARRAY_BUFFER, cubeVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(cubeVertices), cubeVertices, GL_STATIC_DRAW);

    glGenBuffers(1, &cubeUVVBO);
    glBindBuffer(GL_ARRAY_BUFFER, cubeUVVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(cubeUVs), cubeUVs, GL_STATIC_DRAW);

    glGenBuffers(1, &cubeEBO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, cubeEBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(cubeIndices), cubeIndices, GL_STATIC_DRAW);

    // position attribute
    glBindBuffer(GL_ARRAY_BUFFER, cubeVBO);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);

    // normal attribute (offset by 3 floats)
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));

    glBindBuffer(GL_ARRAY_BUFFER, cubeUVVBO);
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 0, (void*)0);

    glBindVertexArray(0);

    unsigned int roomVAO, roomVBO, roomUVVBO, roomEBO;
    glGenVertexArrays(1, &roomVAO);
    glBindVertexArray(roomVAO);

    glGenBuffers(1, &roomVBO);
    glBindBuffer(GL_ARRAY_BUFFER, roomVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(roomVertices), roomVertices, GL_STATIC_DRAW);

    glGenBuffers(1, &roomUVVBO);
    glBindBuffer(GL_ARRAY_BUFFER, roomUVVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(roomUVs), roomUVs, GL_STATIC_DRAW);

    glGenBuffers(1, &roomEBO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, roomEBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(roomIndices), roomIndices, GL_STATIC_DRAW);

    // position attribute
    glBindBuffer(GL_ARRAY_BUFFER, roomVBO);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);

    // normal attribute (offset by 3 floats)
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));

    glBindBuffer(GL_ARRAY_BUFFER, roomUVVBO);
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 0, (void*)0);

    glBindVertexArray(0);

    //loads shaders and assets
    unsigned int shaderID = LoadShaders("vertexShader.glsl", "fragmentshader.glsl");
    unsigned int crateTexture = loadTexture("../assets/crate.jpg");
    unsigned int stoneDiffuse = loadTexture("../assets/stones_diffuse.png");
    unsigned int stoneNormal = loadTexture("../assets/stones_normal.png");
    unsigned int stoneSpecular = loadTexture("../assets/stones_specular.png");
    unsigned int brickDiffuse = loadTexture("../assets/bricks_diffuse.png");
    unsigned int brickNormal = loadTexture("../assets/bricks_normal.png");
    unsigned int brickSpecular = loadTexture("../assets/bricks_specular.png");

    glm::mat4 projection = glm::perspective(glm::radians(camera.getZoom()), 1024.0f / 768.0f, 0.1f, 100.0f);
    spotLight.setDirection(glm::vec3(0.0f, -1.0f, 0.0f));
    light.setColor(glm::vec3(1.0f));

    float lastFrame = 0.0f;

    while (!glfwWindowShouldClose(window))
    {
        float currentFrame = glfwGetTime();
        float deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        keyboardInput(window);
        camera.ProcessKeyboard(window, deltaTime);
        checkCollisions(camera);

        glClearColor(0.2f, 0.2f, 0.2f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glUseProgram(shaderID);
        //spotlight
        glUniform3fv(glGetUniformLocation(shaderID, "lightPos"), 1, glm::value_ptr(light.getPosition()));
        glUniform3fv(glGetUniformLocation(shaderID, "lightColor"), 1, glm::value_ptr(light.getColor()));
        glUniform3fv(glGetUniformLocation(shaderID, "viewPos"), 1, glm::value_ptr(camera.getPosition()));

        glUniform3fv(glGetUniformLocation(shaderID, "spotLightColor"), 1, glm::value_ptr(spotLight.getColor()));
        glUniform3fv(glGetUniformLocation(shaderID, "spotLightPos"), 1, glm::value_ptr(spotLight.getPosition()));
        glUniform3fv(glGetUniformLocation(shaderID, "spotLightDir"), 1, glm::value_ptr(spotLight.getDirection()));
        glUniform1f(glGetUniformLocation(shaderID, "spotCutOff"), spotLight.getCutOff());
        glUniform1f(glGetUniformLocation(shaderID, "spotOuterCutOff"), spotLight.getOuterCutOff());

        
        glm::mat4 cubeModel = glm::mat4(1.0f);
        cubeModel = glm::scale(cubeModel, glm::vec3(0.7f));
        cubeModel = glm::rotate(cubeModel, currentFrame * 0.5f, glm::vec3(1, 1, 0));
        glm::mat4 view = camera.getViewMatrix();
        glm::mat4 cubeMVP = projection * view * cubeModel;

        glUniformMatrix4fv(glGetUniformLocation(shaderID, "MVP"), 1, GL_FALSE, glm::value_ptr(cubeMVP));
        glUniformMatrix4fv(glGetUniformLocation(shaderID, "model"), 1, GL_FALSE, glm::value_ptr(cubeModel));
        glUniform1i(glGetUniformLocation(shaderID, "surfaceType"), 0);

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, crateTexture);
        glUniform1i(glGetUniformLocation(shaderID, "diffuseMap"), 0);
        glUniform1i(glGetUniformLocation(shaderID, "normalMap"), 0);
        glUniform1i(glGetUniformLocation(shaderID, "specularMap"), 0);

        glBindVertexArray(cubeVAO);
        glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);

       
        glm::mat4 roomModel = glm::mat4(1.0f);
        glm::mat4 roomMVP = projection * view * roomModel;

        glUniformMatrix4fv(glGetUniformLocation(shaderID, "MVP"), 1, GL_FALSE, glm::value_ptr(roomMVP));
        glUniformMatrix4fv(glGetUniformLocation(shaderID, "model"), 1, GL_FALSE, glm::value_ptr(roomModel));

        glBindVertexArray(roomVAO);

        //floor
        glUniform1i(glGetUniformLocation(shaderID, "surfaceType"), 1);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, stoneDiffuse);
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, stoneNormal);
        glActiveTexture(GL_TEXTURE2);
        glBindTexture(GL_TEXTURE_2D, stoneSpecular);
        glUniform1i(glGetUniformLocation(shaderID, "diffuseMap"), 0);
        glUniform1i(glGetUniformLocation(shaderID, "normalMap"), 1);
        glUniform1i(glGetUniformLocation(shaderID, "specularMap"), 2);
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, (void*)0);

        //ceiling
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, (void*)(6 * sizeof(unsigned int)));

        //walls
        glUniform1i(glGetUniformLocation(shaderID, "surfaceType"), 2);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, brickDiffuse);
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, brickNormal);
        glActiveTexture(GL_TEXTURE2);
        glBindTexture(GL_TEXTURE_2D, brickSpecular);
        glUniform1i(glGetUniformLocation(shaderID, "diffuseMap"), 0);
        glUniform1i(glGetUniformLocation(shaderID, "normalMap"), 1);
        glUniform1i(glGetUniformLocation(shaderID, "specularMap"), 2);
        glDrawElements(GL_TRIANGLES, 24, GL_UNSIGNED_INT, (void*)(12 * sizeof(unsigned int)));

        //draw spotlight
        glm::mat4 spotModel = glm::translate(glm::mat4(1.0f), spotLight.getPosition());
        spotModel = glm::scale(spotModel, glm::vec3(0.1f));
        glm::mat4 spotMVP = projection * view * spotModel;

        glUniformMatrix4fv(glGetUniformLocation(shaderID, "MVP"), 1, GL_FALSE, glm::value_ptr(spotMVP));
        glUniformMatrix4fv(glGetUniformLocation(shaderID, "model"), 1, GL_FALSE, glm::value_ptr(spotModel));
        glUniform1i(glGetUniformLocation(shaderID, "surfaceType"), 0);

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, crateTexture);
        glUniform1i(glGetUniformLocation(shaderID, "diffuseMap"), 0);
        glUniform1i(glGetUniformLocation(shaderID, "normalMap"), 0);
        glUniform1i(glGetUniformLocation(shaderID, "specularMap"), 0);

        glBindVertexArray(cubeVAO);
        glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);

        glBindVertexArray(0);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    //cleanup of course
    glDeleteVertexArrays(1, &cubeVAO);
    glDeleteVertexArrays(1, &roomVAO);
    glDeleteBuffers(1, &cubeVBO);
    glDeleteBuffers(1, &cubeUVVBO);
    glDeleteBuffers(1, &cubeEBO);
    glDeleteBuffers(1, &roomVBO);
    glDeleteBuffers(1, &roomUVVBO);
    glDeleteBuffers(1, &roomEBO);
    glDeleteProgram(shaderID);
    glDeleteTextures(1, &crateTexture);
    glDeleteTextures(1, &stoneDiffuse);
    glDeleteTextures(1, &stoneNormal);
    glDeleteTextures(1, &stoneSpecular);
    glDeleteTextures(1, &brickDiffuse);
    glDeleteTextures(1, &brickNormal);
    glDeleteTextures(1, &brickSpecular);

    glfwTerminate();
    return 0;
}

void keyboardInput(GLFWwindow* window)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);

    float lightVelocity = LIGHT_SPEED * 0.016f;
    if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS)
        light.setPosition(light.getPosition() + glm::vec3(0, 0, -lightVelocity));
    if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS)
        light.setPosition(light.getPosition() + glm::vec3(0, 0, lightVelocity));
    if (glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS)
        light.setPosition(light.getPosition() + glm::vec3(-lightVelocity, 0, 0));
    if (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS)
        light.setPosition(light.getPosition() + glm::vec3(lightVelocity, 0, 0));
    if (glfwGetKey(window, GLFW_KEY_PAGE_UP) == GLFW_PRESS)
        light.setPosition(light.getPosition() + glm::vec3(0, lightVelocity, 0));
    if (glfwGetKey(window, GLFW_KEY_PAGE_DOWN) == GLFW_PRESS)
        light.setPosition(light.getPosition() + glm::vec3(0, -lightVelocity, 0));
}

void mouseCallback(GLFWwindow* window, double xpos, double ypos)
{
    if (firstMouse)
    {
        lastX = xpos;
        lastY = ypos;
        firstMouse = false;
    }
    float xoffset = xpos - lastX;
    float yoffset = lastY - ypos; // reversed
    lastX = xpos;
    lastY = ypos;

    float sensitivity = 0.1f;
    xoffset *= sensitivity;
    yoffset *= sensitivity;

    camera.ProcessMouseMovement(xoffset, yoffset);
}

void scrollCallback(GLFWwindow* window, double xoffset, double yoffset)
{
    camera.ProcessMouseScroll(yoffset);
}

void checkCollisions(Camera& camera)
{
    glm::vec3 pos = camera.getPosition();
    float radius = 0.5f;

    if (pos.x - radius < -ROOM_WIDTH) pos.x = -ROOM_WIDTH + radius;
    if (pos.x + radius > ROOM_WIDTH) pos.x = ROOM_WIDTH - radius;

    if (pos.y - radius < -ROOM_HEIGHT) pos.y = -ROOM_HEIGHT + radius;
    if (pos.y + radius > ROOM_HEIGHT) pos.y = ROOM_HEIGHT - radius;

    if (pos.z - radius < -ROOM_DEPTH) pos.z = -ROOM_DEPTH + radius;
    if (pos.z + radius > ROOM_DEPTH) pos.z = ROOM_DEPTH - radius;

    camera.setPosition(pos);
}