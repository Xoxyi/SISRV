#include "glad/glad.h"
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <shader_class.h>
#include <camera_class.h>
#include <model_class.h>

#include <iostream>
#include <math.h>

//initial window dimention and aspect ratio
const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;

//initial camera settings
Camera camera(glm::vec3(0.0f, 0.0f, 3.0f));
float lastX = SCR_WIDTH / 2.0f;
float lastY = SCR_HEIGHT / 2.0f;
bool firstMouse = true;

//timing for normalize velocity when different FPS
float deltaTime = 0.0f;	// time between current frame and last frame
float lastFrame = 0.0f;

//light sources
glm::vec3 lightPos(1.2f, 1.0f, 2.0f);

//callback function called on user interaction
void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void processInput(GLFWwindow *window);

//clear windows with a desidered color and reset old buffer
void clear_window_buffer();

// glfw: initialize and configure
GLFWwindow* glfwInitialize(const char* windowName);

void gladInitialize();

int main()
{
    
    GLFWwindow* window = glfwInitialize("Learn OpenGL");

    gladInitialize();

    // tell stb_image.h to flip loaded texture's on the y-axis (before loading model).
    stbi_set_flip_vertically_on_load(true);

    // configure global opengl state (enabling test on z buffer)
    glEnable(GL_DEPTH_TEST);

    // build and compile our shaders program
    Shader shader("shaders/texture.vs", "shaders/texture.fs");
    Shader shaderNormals("shaders/textureNormal.vs", "shaders/texture.gs", "shaders/texture_green.fs");

    // load models
    Model backpack("assets/models/backpack/backpack.obj");

    // draw in wireframe
    //glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

    //render loop
    while (!glfwWindowShouldClose(window)) {

        //take timing
        float currentFrame = static_cast<float>(glfwGetTime());
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        //user input
        processInput(window);

        clear_window_buffer();

        //enabling shader
        shader.use();

        //shader.setFloat("time", glfwGetTime());

        //transformations matrix
        glm::mat4 model = glm::mat4(1.0f);
        glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
        glm::mat4 view = camera.GetViewMatrix();

        // pass matrix to shader
        shader.setMat4("projection", projection);
        shader.setMat4("view", view);
        shader.setMat4("model", model);

        //actual drawing
        backpack.Draw(shader);

        shaderNormals.use();

        shaderNormals.setMat4("projection", projection);
        shaderNormals.setMat4("view", view);
        shaderNormals.setMat4("model", model);

        backpack.Draw(shaderNormals);


        glfwPollEvents();
        glfwSwapBuffers(window);
    }

    shader.deleteProgram();

    glfwTerminate();
    return 0;
}

void processInput(GLFWwindow *window)
{
    //Esc pressed
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);

    //WASD pressed
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        camera.ProcessKeyboard(FORWARD, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        camera.ProcessKeyboard(BACKWARD, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        camera.ProcessKeyboard(LEFT, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        camera.ProcessKeyboard(RIGHT, deltaTime);
}

void mouse_callback(GLFWwindow* window, double xposIn, double yposIn)
{
    float xpos = static_cast<float>(xposIn);
    float ypos = static_cast<float>(yposIn);

    if (firstMouse) {
        lastX = xpos;
        lastY = ypos;
        firstMouse = false;
    }

    float xoffset = xpos - lastX;
    float yoffset = lastY - ypos; // reversed since y-coordinates go from bottom to top

    lastX = xpos;
    lastY = ypos;

    camera.ProcessMouseMovement(xoffset, yoffset);
}

void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
    camera.ProcessMouseScroll(static_cast<float>(yoffset));
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    glViewport(0, 0, width, height);
}

void clear_window_buffer()
{
    glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

GLFWwindow* glfwInitialize(const char * windowName)
{
    // glfw: initialize and configure
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    // glfw window creation 
    GLFWwindow* window = glfwCreateWindow(800, 600, windowName, NULL, NULL);
    if (window == NULL) {
        std::cerr << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        exit(1);
    }
    glfwMakeContextCurrent(window);
    
    //register callback function for resize windows
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

    //register callback function for mouse
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    glfwSetCursorPosCallback(window, mouse_callback);
    glfwSetScrollCallback(window, scroll_callback);
    
    return window;
}

void gladInitialize()
{
    // glad: load all OpenGL function pointers
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        std::cerr << "Failed to initialize GLAD" << std::endl;
        exit(1);
    }
}