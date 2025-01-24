#include <glad/glad.h>
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
const unsigned int SCR_WIDTH = 1200;
const unsigned int SCR_HEIGHT = 900;

//initial camera settings
Camera camera(glm::vec3(0.0f, 100.0f, 50.0f));
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

unsigned int load_cubemap();
unsigned int gen_skybox_VAO();
void gen_trasformation(int amount, glm::vec3 *displacement, glm::vec2 *scale_rotation);
void bind_rock_VAO(Model rock, int amount, glm::mat4 *modelMatrices);
void update_rock_position(Model rock, int amount, glm::vec3 *displacement, glm::vec2 *scale_rotation);


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
    Shader instanceShader("shaders/textureIsntaced.vs", "shaders/texture.fs");
    Shader skyboxShader("shaders/skybox.vs", "shaders/skybox.fs");

    // load models
    Model planet("assets/models/planet/planet.obj");
    Model rock("assets/models/rock/rock.obj");


    unsigned int cubemapTexture = load_cubemap();     

    unsigned int skyboxVAO = gen_skybox_VAO();

    skyboxShader.use();
    skyboxShader.setInt("skybox", 0);

    int amount = 10000;
    glm::vec3 *displacement;
    glm::vec2 *scale_rotation;
    displacement = new glm::vec3[amount];
    scale_rotation = new glm::vec2[amount];
    gen_trasformation(amount, displacement, scale_rotation);
    update_rock_position(rock, amount, displacement, scale_rotation);
    

    // draw in wireframe
    //glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    

    //render loop
    while (!glfwWindowShouldClose(window)) {

        //take timing
        float currentFrame = static_cast<float>(glfwGetTime());
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;
        std::cout << 1/deltaTime << std::endl;

        //user input
        processInput(window);

        clear_window_buffer();

        //enabling shader
        shader.use();
        
        // draw planet
        shader.use();
        glm::mat4 model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(0.0f, -3.0f, 0.0f));
        model = glm::scale(model, glm::vec3(4.0f, 4.0f, 4.0f));
        glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 500.0f);
        glm::mat4 view = camera.GetViewMatrix();

        // pass matrix to shader
        shader.setMat4("projection", projection);
        shader.setMat4("view", view);
        shader.setMat4("model", model);

        //actual drawing
        planet.Draw(shader);
        /*
        for(unsigned int i = 0; i < amount; i++)
        {
            shader.setMat4("model", modelMatrices[i]);
            rock.Draw(shader);
        }  */



        instanceShader.use();

        update_rock_position(rock, amount, displacement, scale_rotation);
        instanceShader.setMat4("projection", projection);
        instanceShader.setMat4("view", view);
        for(unsigned int i = 0; i < rock.meshes.size(); i++)
        {
            glBindVertexArray(rock.meshes[i].VAO);
            glDrawElementsInstanced(GL_TRIANGLES, rock.meshes[i].indices.size(), GL_UNSIGNED_INT, 0, amount);
        }

        // draw skybox as last
        glDepthFunc(GL_LEQUAL);  // change depth function so depth test passes when values are equal to depth buffer's content
        skyboxShader.use();
        view = glm::mat4(glm::mat3(camera.GetViewMatrix())); // remove translation from the view matrix
        skyboxShader.setMat4("view", view);
        skyboxShader.setMat4("projection", projection);
        // skybox cube
        glBindVertexArray(skyboxVAO);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_CUBE_MAP, cubemapTexture);
        glDrawArrays(GL_TRIANGLES, 0, 36);
        glBindVertexArray(0);
        glDepthFunc(GL_LESS);

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
    GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, windowName, NULL, NULL);
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

unsigned int load_cubemap()
{
    unsigned int textureID;
    glGenTextures(1, &textureID);
    glBindTexture(GL_TEXTURE_CUBE_MAP, textureID);
    stbi_set_flip_vertically_on_load(false);
    std::vector<std::string> faces =
    {
        "right.jpg",
        "left.jpg",
        "top.jpg",
        "bottom.jpg",
        "front.jpg",
        "back.jpg"
    };

    int width, height, nrChannels;
    for (unsigned int i = 0; i < faces.size(); i++)
    {
        unsigned char *data = stbi_load(("assets/textures/universe/" + faces[i]).c_str(), &width, &height, &nrChannels, 0);
        if (data)
        {
            glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 
                         0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data
            );
            stbi_image_free(data);
        }
        else
        {
            std::cout << "Cubemap tex failed to load at path: " << faces[i] << std::endl;
            stbi_image_free(data);
        }
    }
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
    stbi_set_flip_vertically_on_load(true);
    return textureID;
}

unsigned int gen_skybox_VAO()
{
    float skyboxVertices[] = {
        // positions          
        -1.0f,  1.0f, -1.0f,
        -1.0f, -1.0f, -1.0f,
         1.0f, -1.0f, -1.0f,
         1.0f, -1.0f, -1.0f,
         1.0f,  1.0f, -1.0f,
        -1.0f,  1.0f, -1.0f,

        -1.0f, -1.0f,  1.0f,
        -1.0f, -1.0f, -1.0f,
        -1.0f,  1.0f, -1.0f,
        -1.0f,  1.0f, -1.0f,
        -1.0f,  1.0f,  1.0f,
        -1.0f, -1.0f,  1.0f,

         1.0f, -1.0f, -1.0f,
         1.0f, -1.0f,  1.0f,
         1.0f,  1.0f,  1.0f,
         1.0f,  1.0f,  1.0f,
         1.0f,  1.0f, -1.0f,
         1.0f, -1.0f, -1.0f,

        -1.0f, -1.0f,  1.0f,
        -1.0f,  1.0f,  1.0f,
         1.0f,  1.0f,  1.0f,
         1.0f,  1.0f,  1.0f,
         1.0f, -1.0f,  1.0f,
        -1.0f, -1.0f,  1.0f,

        -1.0f,  1.0f, -1.0f,
         1.0f,  1.0f, -1.0f,
         1.0f,  1.0f,  1.0f,
         1.0f,  1.0f,  1.0f,
        -1.0f,  1.0f,  1.0f,
        -1.0f,  1.0f, -1.0f,

        -1.0f, -1.0f, -1.0f,
        -1.0f, -1.0f,  1.0f,
         1.0f, -1.0f, -1.0f,
         1.0f, -1.0f, -1.0f,
        -1.0f, -1.0f,  1.0f,
         1.0f, -1.0f,  1.0f
    };

    // skybox VAO
    unsigned int skyboxVAO, skyboxVBO;
    glGenVertexArrays(1, &skyboxVAO);
    glGenBuffers(1, &skyboxVBO);
    glBindVertexArray(skyboxVAO);
    glBindBuffer(GL_ARRAY_BUFFER, skyboxVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(skyboxVertices), &skyboxVertices, GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    return skyboxVAO;
}

void gen_trasformation(int amount, glm::vec3 *displacement, glm::vec2 *scale_rotation)
{
    float offset = 15.0f;
    srand(glfwGetTime()); // initialize random seed	
    for(int i = 0; i < amount; i++)
    {
        // 1. translation: displace along circle with 'radius' in range [-offset, offset]
        displacement[i].x = (rand() % (int)(2 * offset * 100)) / 100.0f - offset;
        displacement[i].y = (rand() % (int)(2 * offset * 100)) / 100.0f - offset;
        displacement[i].z = (rand() % (int)(2 * offset * 100)) / 100.0f - offset;

        // 2. scale: scale between 0.05 and 0.25f
        scale_rotation[i].x = (rand() % 20) / 100.0f + 0.05;

        // 3. rotation: add random rotation around a (semi)randomly picked rotation axis vector
        scale_rotation[i].y  = (rand() % 360);

    }
}

void update_rock_position(Model rock, int amount, glm::vec3 *displacement, glm::vec2 *scale_rotation)
{
    glm::mat4 *modelMatrices;
    modelMatrices = new glm::mat4[amount];
    float radius = 75.0f;
    float angularSpeed = .5f;
    for(int i = 0; i < amount; i++)
    {
        glm::mat4 model = glm::mat4(1.0f);
        float angle = (float)i / (float)amount * 360.0f + angularSpeed * glfwGetTime();
        float x = sin(angle) * radius + displacement[i].x;
        float y = displacement[i].y * 0.4f; // keep height of field smaller compared to width of x and z
        float z = cos(angle) * radius + displacement[i].z;
        model = glm::translate(model, glm::vec3(x, y, z));

        // 2. scale: scale between 0.05 and 0.25f
        model = glm::scale(model, glm::vec3(scale_rotation[i].x));

        // 3. rotation: add random rotation around a (semi)randomly picked rotation axis vector
        model = glm::rotate(model, scale_rotation[i].y, glm::vec3(0.4f, 0.6f, 0.8f));

        // 4. now add to list of matrices
        modelMatrices[i] = model;
    }
    bind_rock_VAO(rock, amount, modelMatrices);
}

void bind_rock_VAO(Model rock, int amount, glm::mat4 *modelMatrices)
{
    // vertex buffer object
    unsigned int buffer;
    glGenBuffers(1, &buffer);
    glBindBuffer(GL_ARRAY_BUFFER, buffer);
    glBufferData(GL_ARRAY_BUFFER, amount * sizeof(glm::mat4), &modelMatrices[0], GL_STATIC_DRAW);
    
    for(unsigned int i = 0; i < rock.meshes.size(); i++)
    {
        unsigned int VAO = rock.meshes[i].VAO;
        glBindVertexArray(VAO);
        // vertex attributes
        std::size_t vec4Size = sizeof(glm::vec4);
        glEnableVertexAttribArray(3); 
        glVertexAttribPointer(3, 4, GL_FLOAT, GL_FALSE, 4 * vec4Size, (void*)0);
        glEnableVertexAttribArray(4); 
        glVertexAttribPointer(4, 4, GL_FLOAT, GL_FALSE, 4 * vec4Size, (void*)(1 * vec4Size));
        glEnableVertexAttribArray(5); 
        glVertexAttribPointer(5, 4, GL_FLOAT, GL_FALSE, 4 * vec4Size, (void*)(2 * vec4Size));
        glEnableVertexAttribArray(6); 
        glVertexAttribPointer(6, 4, GL_FLOAT, GL_FALSE, 4 * vec4Size, (void*)(3 * vec4Size));

        glVertexAttribDivisor(3, 1);
        glVertexAttribDivisor(4, 1);
        glVertexAttribDivisor(5, 1);
        glVertexAttribDivisor(6, 1);

        glBindVertexArray(0);
    }
}