#ifndef LIGHTING_PASS_H
#define LIGHTING_PASS_H

#include "camera_class.h"
#include "frame_buffer_class.h"
#include "g_buffer_class.h"
#include "model_class.h"
#include "object_class.h"
#include "shader_class.h"
#include "texture_class.h"
#include <iostream>
#include <vector>

class LightingPass
{
public:
    GBuffer *gBuffer;
    LightingPass(GBuffer *gBuffer);
    void lightingPass(Shader &shader);
    void lightingPass(Shader &phongShader, Shader &lightShader);
    Model quad;

};

LightingPass::LightingPass(GBuffer *gBuffer) : gBuffer(gBuffer), quad(Model::GenQuad()){}

void LightingPass::lightingPass(Shader &phongShader, Shader &lightShader)
{
    phongShader.use();
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, gBuffer->position.id);
    phongShader.setInt("gPosition", 0);
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, gBuffer->normal.id);
    phongShader.setInt("gNormal", 1);
    glActiveTexture(GL_TEXTURE2);
    glBindTexture(GL_TEXTURE_2D, gBuffer->albedo.id);
    phongShader.setInt("gAlbedo", 2);
    glActiveTexture(GL_TEXTURE3);
    glBindTexture(GL_TEXTURE_2D, gBuffer->reflection.id);
    phongShader.setInt("gReflection", 3);
    phongShader.setVec3("viewPos", camera.Position);
    quad.Draw(phongShader);

    //
    //

    lightShader.use();
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, gBuffer->albedo.id);
    lightShader.setInt("gAlbedo", 0);
    quad.Draw(lightShader);
}


void LightingPass::lightingPass(Shader &shader)
{
    shader.use();
    //Texture tex = Texture("assets/textures/wood.png", ".", true);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, gBuffer->position.id);
    shader.setInt("gPosition", 0);
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, gBuffer->normal.id);
    shader.setInt("gNormal", 1);
    glActiveTexture(GL_TEXTURE2);
    glBindTexture(GL_TEXTURE_2D, gBuffer->albedo.id);
    shader.setInt("gAlbedo", 2);
    glActiveTexture(GL_TEXTURE3);
    glBindTexture(GL_TEXTURE_2D, gBuffer->reflection.id);
    shader.setInt("gReflection", 3);
    shader.setVec3("viewPos", camera.Position);
    quad.Draw(shader);
}

#endif
