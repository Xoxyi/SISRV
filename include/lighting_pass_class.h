#ifndef LIGHTING_PASS_H
#define LIGHTING_PASS_H

#include "camera_class.h"
#include "cube_map_class.h"
#include "frame_buffer_class.h"
#include "g_buffer_class.h"
#include "model_class.h"
#include "object_class.h"
#include "shader_class.h"
#include "sky_box.h"
#include "texture_class.h"
#include <iostream>
#include <vector>
#include <shadow_map_class.h>
#include <string>
#include <shadow_map_array_class.h>

class LightingPass
{
public:
    GBuffer *gBuffer;
    LightingPass(GBuffer *gBuffer);
    void lightingPass(Shader &shader);
    void lightingPass(Shader &phongShader, Shader &lightShader, Shader& pbrShader, SkyBox skyBox, ShadowMapArray& shadowMaps);
    Model quad;

};

LightingPass::LightingPass(GBuffer *gBuffer) : gBuffer(gBuffer), quad(Model::GenQuad()){}

void LightingPass::lightingPass(Shader &phongShader, Shader &lightShader, Shader& pbrShader, SkyBox skyBox, ShadowMapArray& shadowMaps)
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

    glActiveTexture(GL_TEXTURE4);
    glBindTexture(GL_TEXTURE_CUBE_MAP, skyBox.irradianceMap.id);
    phongShader.setInt("irradianceMap", 4);

    glActiveTexture(GL_TEXTURE5);
    glBindTexture(GL_TEXTURE_CUBE_MAP_ARRAY, shadowMaps.ID);
    phongShader.setInt("depthMaps", 5);

    phongShader.setVec3("viewPos", camera.Position);
    quad.Draw(phongShader);

    //
    //

    lightShader.use();
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, gBuffer->albedo.id);
    lightShader.setInt("gAlbedo", 0);
    quad.Draw(lightShader);

    //
    //

    pbrShader.use();
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, gBuffer->position.id);
    pbrShader.setInt("gPosition", 0);
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, gBuffer->normal.id);
    pbrShader.setInt("gNormal", 1);
    glActiveTexture(GL_TEXTURE2);
    glBindTexture(GL_TEXTURE_2D, gBuffer->albedo.id);
    pbrShader.setInt("gAlbedo", 2);
    glActiveTexture(GL_TEXTURE3);
    glBindTexture(GL_TEXTURE_2D, gBuffer->reflection.id);
    pbrShader.setInt("gReflection", 3);

    glActiveTexture(GL_TEXTURE4);
    glBindTexture(GL_TEXTURE_CUBE_MAP, skyBox.irradianceMap.id);
    pbrShader.setInt("irradianceMap", 4);

    glActiveTexture(GL_TEXTURE5);
    glBindTexture(GL_TEXTURE_CUBE_MAP_ARRAY, shadowMaps.ID);
    pbrShader.setInt("depthMaps", 5);

    pbrShader.setVec3("viewPos", camera.Position);
    quad.Draw(pbrShader);
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
