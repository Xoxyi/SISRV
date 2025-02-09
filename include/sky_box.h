#ifndef SKY_BOX_H
#define SKY_BOX_H

#include "camera_class.h"
#include "cube_map_class.h"
#include "frame_buffer_class.h"
#include "model_class.h"
#include "shader_class.h"
#include "texture_class.h"
#include <iostream>

class SkyBox {
public: 

    CubeMap cubeMap;

    CubeMap irradianceMap;

    SkyBox(const char *path);

};

SkyBox::SkyBox(const char *path) : cubeMap(GL_CLAMP_TO_EDGE, 512, 512, GL_RGB16F, GL_RGB, GL_FLOAT), irradianceMap(GL_CLAMP_TO_EDGE, 32, 32, GL_RGB16F, GL_RGB, GL_FLOAT)
{
    Shader equirectangularToCubemapShader("shaders/cubemap.vs", "shaders/equirectangular_to_cubemap.fs");
    Shader irradianceShader("shaders/cubemap.vs", "shaders/irradiance_convolution.fs");

    Texture planeTexture = Texture(path, GL_CLAMP_TO_EDGE, GL_RGB16F, GL_RGB, GL_FLOAT);
    FrameBuffer captureBuffer = FrameBuffer();
    Model cube = Model::GenCube();

    captureBuffer.addDephAttachment(512, 512);
    glm::mat4 captureProjection = glm::perspective(glm::radians(90.0f), 1.0f, 0.1f, 10.0f);
    glm::mat4 captureViews[] =
    {
        glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(1.0f,  0.0f,  0.0f), glm::vec3(0.0f, -1.0f,  0.0f)),
        glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(-1.0f,  0.0f,  0.0f), glm::vec3(0.0f, -1.0f,  0.0f)),
        glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f,  1.0f,  0.0f), glm::vec3(0.0f,  0.0f,  1.0f)),
        glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, -1.0f,  0.0f), glm::vec3(0.0f,  0.0f, -1.0f)),
        glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f,  0.0f,  1.0f), glm::vec3(0.0f, -1.0f,  0.0f)),
        glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f,  0.0f, -1.0f), glm::vec3(0.0f, -1.0f,  0.0f))
    };
    std::cout << __FILE__ << " " << __func__ << " " << __LINE__ << std::endl;


    equirectangularToCubemapShader.use();
    equirectangularToCubemapShader.setInt("equirectangularMap", 0);
    equirectangularToCubemapShader.setMat4("projection", captureProjection);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, planeTexture.id);


    
    glViewport(0, 0, 512, 512);
    std::cout << __FILE__ << " " << __func__ << " " << __LINE__ << std::endl;
    captureBuffer.enable();
    for (unsigned int i = 0; i < 6; ++i)
    {
        equirectangularToCubemapShader.setMat4("view", captureViews[i]);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, cubeMap.id, 0);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        cube.Draw(equirectangularToCubemapShader);
    }

    glBindTexture(GL_TEXTURE_CUBE_MAP, cubeMap.id);
    glGenerateMipmap(GL_TEXTURE_CUBE_MAP);
    std::cout << __FILE__ << " " << __func__ << " " << __LINE__ << std::endl;

    captureBuffer.addDephAttachment(32,32);
    irradianceShader.use();
    irradianceShader.setInt("environmentMap", 0);
    irradianceShader.setMat4("projection", captureProjection);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_CUBE_MAP, cubeMap.id);

    glViewport(0, 0, 32, 32); // don't forget to configure the viewport to the capture dimensions.
    
    captureBuffer.enable();
    for (unsigned int i = 0; i < 6; ++i)
    {
        irradianceShader.setMat4("view", captureViews[i]);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, irradianceMap.id, 0);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        cube.Draw(irradianceShader);
    }
    glBindTexture(GL_TEXTURE_CUBE_MAP, irradianceMap.id);
    glGenerateMipmap(GL_TEXTURE_CUBE_MAP);
    captureBuffer.disable();
    glViewport(0, 0, SCR_WIDTH, SCR_HEIGHT);
}

#endif