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

    CubeMap envMap;

    CubeMap irradianceMap;

    CubeMap speccularMap;

    SkyBox(const char *path);

private:

    void genEnvMap(const char *path);
    void genIrradianceMap();
    void genSpecularMap();

};

SkyBox::SkyBox(const char *path) :  envMap(GL_CLAMP_TO_EDGE, 512, 512, GL_RGB16F, GL_RGB, GL_FLOAT), 
                                    irradianceMap(GL_CLAMP_TO_EDGE, 32, 32, GL_RGB16F, GL_RGB, GL_FLOAT),
                                    speccularMap(GL_CLAMP_TO_EDGE, 32, 32, GL_RGB16F, GL_RGB, GL_FLOAT)

{

    genEnvMap(path);

    genIrradianceMap();

    genSpecularMap();

    glViewport(0, 0, SCR_WIDTH, SCR_HEIGHT);
    
}

void SkyBox::genEnvMap(const char *path)
{
    Shader equirectangularToCubemapShader("shaders/cubemap.vs", "shaders/equirectangular_to_cubemap.fs");
    

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

    equirectangularToCubemapShader.use();
    equirectangularToCubemapShader.setInt("equirectangularMap", 0);
    equirectangularToCubemapShader.setMat4("projection", captureProjection);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, planeTexture.id);

    glViewport(0, 0, 512, 512);

    captureBuffer.enable();
    for (unsigned int i = 0; i < 6; ++i)
    {
        equirectangularToCubemapShader.setMat4("view", captureViews[i]);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, envMap.id, 0);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        cube.Draw(equirectangularToCubemapShader);
    }

    glBindTexture(GL_TEXTURE_CUBE_MAP, envMap.id);
    glGenerateMipmap(GL_TEXTURE_CUBE_MAP);
    captureBuffer.disable();
}

void SkyBox::genIrradianceMap()
{
    Shader irradianceShader("shaders/cubemap.vs", "shaders/irradiance_convolution.fs");
    
    FrameBuffer captureBuffer = FrameBuffer();
    captureBuffer.addDephAttachment(32,32);

    Model cube = Model::GenCube();
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

    irradianceShader.use();
    irradianceShader.setInt("environmentMap", 0);
    irradianceShader.setMat4("projection", captureProjection);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_CUBE_MAP, envMap.id);

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
}

void SkyBox::genSpecularMap()
{
    //TODO
}




#endif