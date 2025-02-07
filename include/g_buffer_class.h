#ifndef G_BUFFER_H
#define G_BUFFER_H

#include "camera_class.h"
#include "frame_buffer_class.h"
#include "object_class.h"
#include "shader_class.h"
#include "texture_class.h"
#include <iostream>
#include <vector>
#include "scene_class.h"

class GBuffer
{
public:
    FrameBuffer buffer;
    Texture position;
    Texture normal;
    Texture albedo;
    Texture reflection;     //specular, metalness, roughtness, type
    Texture deph;

    GBuffer();

    void geometryPass(Scene& scene, Shader &phongShader, Shader& lightShader);

};

GBuffer::GBuffer() :    buffer(), 
                        position(SCR_WIDTH, SCR_HEIGHT, GL_RGBA16F, GL_RGBA, GL_FLOAT),
                        normal(SCR_WIDTH, SCR_HEIGHT, GL_RGBA16F, GL_RGBA, GL_FLOAT),
                        albedo(SCR_WIDTH, SCR_HEIGHT, GL_RGBA, GL_RGBA, GL_UNSIGNED_BYTE),
                        reflection(SCR_WIDTH, SCR_HEIGHT, GL_RGBA, GL_RGBA, GL_UNSIGNED_BYTE),
                        deph(SCR_WIDTH, SCR_HEIGHT, GL_DEPTH_COMPONENT, GL_DEPTH_COMPONENT, GL_FLOAT)


{
    buffer.addColorAttchment(&position, 0);
    buffer.addColorAttchment(&normal, 1);
    buffer.addColorAttchment(&albedo, 2);
    buffer.addColorAttchment(&reflection, 3);
    buffer.addDepthAttahcment(&deph);
    buffer.updateAttachment(std::vector<unsigned int>{0,1,2,3});
    buffer.checkCompleteness();
}

void GBuffer::geometryPass(Scene& scene, Shader &phongShader, Shader& lightShader)
{
    buffer.enable();
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    scene.DrawPhong(phongShader);
    scene.DrawLight(lightShader);
    buffer.disable();
}

#endif
