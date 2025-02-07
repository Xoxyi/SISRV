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
    Model quad;

};

LightingPass::LightingPass(GBuffer *gBuffer) : gBuffer(gBuffer), quad(Model::GenQuad()){}


void LightingPass::lightingPass(Shader &shader)
{
    shader.use();
    //Texture tex = Texture("assets/textures/wood.png", ".", true);
    glActiveTexture(GL_TEXTURE0);

    glBindTexture(GL_TEXTURE_2D, gBuffer->position.id);

    shader.setInt("gPosition", 0);
    quad.Draw(shader);
}

#endif
