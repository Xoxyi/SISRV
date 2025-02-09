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

    CubeMap prefilterMap;

    Texture brdfLUTTexture;

    SkyBox(const char *path);

private:

    void genEnvMap(const char *path);
    void genIrradianceMap();
    void genSpecularMap();

};

SkyBox::SkyBox(const char* path) : envMap(GL_CLAMP_TO_EDGE, 512, 512, GL_RGB16F, GL_RGB, GL_FLOAT),
irradianceMap(GL_CLAMP_TO_EDGE, 32, 32, GL_RGB16F, GL_RGB, GL_FLOAT),
prefilterMap(GL_CLAMP_TO_EDGE, 128, 128, GL_RGB16F, GL_RGB, GL_FLOAT),
brdfLUTTexture(512, 512, GL_RG16F, GL_RG, GL_FLOAT, GL_LINEAR)

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
    Model cube = Model::GenCube();

    Shader prefilterShader("shaders/cubemap.vs", "shaders/prefilter.fs");
    prefilterShader.use();
    prefilterShader.setInt("environmentMap", 0);
    prefilterShader.setMat4("projection", captureProjection);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_CUBE_MAP, envMap.id);

    FrameBuffer captureBuffer = FrameBuffer();
    captureBuffer.enable();
    unsigned int maxMipLevels = 5;
    for (unsigned int mip = 0; mip < maxMipLevels; ++mip)
    {
        // reisze framebuffer according to mip-level size.
        unsigned int mipWidth = static_cast<unsigned int>(128 * std::pow(0.5, mip));
        unsigned int mipHeight = static_cast<unsigned int>(128 * std::pow(0.5, mip));
        captureBuffer.addDephAttachment(mipWidth, mipHeight);
        captureBuffer.enable();
        glViewport(0, 0, mipWidth, mipHeight);

        float roughness = (float)mip / (float)(maxMipLevels - 1);
        prefilterShader.setFloat("roughness", roughness);
        for (unsigned int i = 0; i < 6; ++i)
        {
            prefilterShader.setMat4("view", captureViews[i]);
            glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, prefilterMap.id, mip);

            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
            cube.Draw(prefilterShader);
        }
    }

    Shader brdfShader("shaders/lighting.vs", "shaders/brdf.fs");
    Model quad = Model::GenQuad();

    captureBuffer.addDephAttachment(512, 512);
    captureBuffer.enable();
    captureBuffer.addColorAttchment(&brdfLUTTexture, 0);
    captureBuffer.enable();
    glViewport(0, 0, 512, 512);
    brdfShader.use();
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    quad.Draw(brdfShader);



    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}




#endif