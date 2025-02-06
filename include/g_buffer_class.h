#ifndef G_BUFFER_H
#define G_BUFFER_H

#include "camera_class.h"
#include "frame_buffer_class.h"
#include "texture_class.h"

class GBuffer
{
public:
    FrameBuffer buffers;
    Texture position;
    Texture normal;
    Texture albedo;
    Texture reflection;     //specular, metalness, roughtness, type
    Texture deph;

    GBuffer();

};

GBuffer::GBuffer() :    buffers(), 
                        position(SCR_WIDTH, SCR_HEIGHT, GL_RGBA16F, GL_RGBA, GL_FLOAT),
                        normal(SCR_WIDTH, SCR_HEIGHT, GL_RGBA16F, GL_RGBA, GL_FLOAT),
                        albedo(SCR_WIDTH, SCR_HEIGHT, GL_RGBA, GL_RGBA, GL_UNSIGNED_BYTE),
                        reflection(SCR_WIDTH, SCR_HEIGHT, GL_RGBA, GL_RGBA, GL_UNSIGNED_BYTE),
                        deph(SCR_WIDTH, SCR_HEIGHT, GL_DEPTH_COMPONENT, GL_DEPTH_COMPONENT, GL_FLOAT)


{
    buffers.addColorAttchment(&position, 0);
    buffers.addColorAttchment(&normal, 1);
    buffers.addColorAttchment(&albedo, 2);
    buffers.addColorAttchment(&reflection, 3);
    buffers.addDepthAttahcment(&deph);
}

#endif
