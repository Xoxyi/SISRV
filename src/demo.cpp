#include "cube_map_class.h"
#include "frame_buffer_class.h"
#include "glm/ext/matrix_transform.hpp"
#include "glm/ext/vector_float3.hpp"
#include "lighting_pass_class.h"
#include "scene_class.h"
#include "sky_box.h"
#include <cstddef>
#include <cstdio>
#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <iterator>
#include <ostream>
#include <shader_class.h>
#include <camera_class.h>
#include <model_class.h>

#include <iostream>
#include <math.h>
#include <vector>
#include <object_class.h>
#include <g_buffer_class.h>
#include <point_light_class.h>
#include <shadow_map_class.h>
#include <shadow_map_array_class.h>
#include <utility.h>

std::vector<Model> models;

void eightMove(Scene* scene);

void moveLight(Scene* scene, GLFWwindow* window, int index);


int main()
{
    GLFWwindow* window = glfwInitialize("Learn OpenGL");

    gladInitialize();

    // configure global opengl state (enabling test on z buffer)
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LEQUAL);


    // build and compile shaders
    // -------------------------
    Shader objGeometryShader("shaders/obj_geometry.vs", "shaders/obj_geometry.fs");
    Shader pbrGeometryShader("shaders/obj_geometry.vs", "shaders/pbr_geometry.fs");
    Shader lightGeometryShader("shaders/light_geometry.vs", "shaders/light_geometry.fs");
    Shader phongLightingShader("shaders/lighting.vs", "shaders/phong_lighting.fs");
    Shader pbrLightingShader("shaders/lighting.vs", "shaders/pbr_lighting.fs");
    Shader lightLightingShader("shaders/lighting.vs", "shaders/light_lighting.fs");
    Shader skyBoxShader("shaders/sky_box.vs", "shaders/sky_box.fs");

    // gen sky box (env map, irradiance map, specular map)
    // ---------------------------------------------------
    stbi_set_flip_vertically_on_load(true);
    SkyBox skyBox("assets/textures/hdr/newport_loft.hdr");
    stbi_set_flip_vertically_on_load(false);
    // load model
    // ----------
    models.emplace_back("assets/models/demo/env/env.obj");
    models.emplace_back("assets/models/demo/plane/plane.obj");

    // set object initial transform
    // ----------------------------
    Object env = Object{ models[0], glm::scale(glm::mat4(1), glm::vec3(.3, .3, .3)) };
    Object plane = Object{ models[1], glm::scale(glm::translate(glm::mat4(1), glm::vec3(0,1,0)), glm::vec3(.005)) };

    // insert oobjects in appropriate vector
    // -------------------------------------
    std::vector<Object> objects;
    objects.push_back(plane);

    std::vector<Object> pbrObjects;
    pbrObjects.push_back(env);

    // set light and light position
    // ----------------------------
    std::vector<PointLight> pointLights;
    pointLights.emplace_back(glm::vec3(1.0, 1.2, 1.0), glm::vec3(20.0, 15.0, 7.0), 1.0f, .0f, .0f);
    //pointLights.emplace_back(glm::vec3(-1.0, 1.0, -1.0), glm::vec3(5.0, 5.0, 5.0), 1.0f, .0f, .0f);
    pointLights.emplace_back(glm::vec3(0.0, 3.0, 0.0), glm::vec3(15.0, 10.0, 5.0), 1.0f, .0f, .0f);

    Scene scene(objects, pbrObjects, pointLights);


    std::vector<ShadowMap> shadowMaps;


    for (auto& pointlight : scene.pointLights)
    {
        shadowMaps.emplace_back(&pointlight);
    }

    GBuffer gBuffer = GBuffer();
    LightingPass lightingPass = LightingPass(&gBuffer);

    //glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

    Model cube = Model::GenCube();
    while (!glfwWindowShouldClose(window))
    {
        // per-frame time logic
        // --------------------
        float currentFrame = static_cast<float>(glfwGetTime());
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;
        scene.update();

        // input
        // -----
        processInput(window);

        // render
        // ------
        //eightMove(&scene);
        for (ShadowMap& shadowMap : shadowMaps)
        {
            shadowMap.Draw(scene);
        }
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        gBuffer.geometryPass(scene, objGeometryShader, lightGeometryShader, pbrGeometryShader);
        glBindFramebuffer(GL_FRAMEBUFFER, 0);

        lightingPass.lightingPass(phongLightingShader, lightLightingShader, pbrLightingShader, skyBox, shadowMaps);
        glClear(GL_DEPTH_BUFFER_BIT);

        glBindFramebuffer(GL_READ_FRAMEBUFFER, gBuffer.buffer.ID);
        glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0); // write to default framebuffer
        glBlitFramebuffer(
            0, 0, SCR_WIDTH, SCR_HEIGHT, 0, 0, SCR_WIDTH, SCR_HEIGHT, GL_DEPTH_BUFFER_BIT, GL_NEAREST
        );
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        skyBoxShader.use();
        skyBoxShader.setMat4("projection", camera.GetProjectionMatrix());
        skyBoxShader.setMat4("view", camera.GetViewMatrix());
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_CUBE_MAP, skyBox.envMap.id);
        cube.Draw(skyBoxShader);
        //lightingPass.lightingPass(phongLightingShader);
        //lightingPass.lightingPass(lightLightingShader);

        // glfw: swap buffers and poll IO events (keys pressed/released, mouse moved etc.)
        // -------------------------------------------------------------------------------
        moveLight(&scene, window, 0);
        glfwSwapBuffers(window);
        glfwPollEvents();
    }


    glfwTerminate();
    return 0;
}


void moveLight(Scene* scene, GLFWwindow* window, int index)
{
    index = 0;
    if (glfwGetKey(window, GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS)
        index = 1;

    
    //WASD pressed
    if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS)
        scene->pointLights[index].position = scene->pointLights[index].position + glm::vec3(0, .1, 0);
    if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS)
        scene->pointLights[index].position = scene->pointLights[index].position + glm::vec3(0, -.1, 0);
    if (glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS)
        scene->pointLights[index].position = scene->pointLights[index].position + glm::vec3(-.1, 0, 0);
    if (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS)
        scene->pointLights[index].position = scene->pointLights[index].position + glm::vec3(.1, 0, 0);
    if (glfwGetKey(window, GLFW_KEY_PAGE_DOWN) == GLFW_PRESS)
        scene->pointLights[index].position = scene->pointLights[index].position + glm::vec3(0, 0, -.1);
    if (glfwGetKey(window, GLFW_KEY_PAGE_UP) == GLFW_PRESS)
        scene->pointLights[index].position = scene->pointLights[index].position + glm::vec3(0, 0, .1);

    

    std::cout << scene->pointLights[index].position.x << std::endl;
    std::cout << scene->pointLights[index].position.y << std::endl;
    std::cout << scene->pointLights[index].position.z << std::endl;
}

void eightMove(Scene* scene)
{
    float radius = .5;
    float time = glfwGetTime();
    float angularVel = 1;
    float x = radius * sin(time * angularVel);
    float z = radius * sin(time * angularVel) * cos(time * angularVel);

    float vx = radius * cos(time * angularVel);
    float vz = radius * (cos(time * angularVel) * cos(time * angularVel) - sin(time * angularVel) * sin(time * angularVel));

    float dir = std::atan2(vx, vz);

    glm::mat4 transform = glm::rotate(glm::scale(glm::translate(glm::mat4(1), glm::vec3(x, 1.1, z)), glm::vec3(0.005)), dir, glm::vec3(0, 1, 0));
    scene->phongObjects[1].transform = transform;
}