#ifndef CUBE_MAP_H
#define CUBE_MAP_H

#include "glad/glad.h"

#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"

#include "stb_image.h"

#include <iostream>
#include <string>

class CubeMap {
public:

    unsigned int id;

    CubeMap(int wrapMethod, int width, int height, int internalFormat, int format, int storageType, int filterMin = GL_LINEAR_MIPMAP_LINEAR, int filterMax = GL_LINEAR);

};



CubeMap::CubeMap(int wrapMethod, int width, int height, int internalFormat, int format, int storageType, int filterMin, int filterMax)
{

    unsigned int textureID;

    glGenTextures(1, &textureID);
    glBindTexture(GL_TEXTURE_CUBE_MAP, textureID);
    for (unsigned int i = 0; i < 6; ++i)
    {
        glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, internalFormat, width, height, 0, format, storageType, nullptr);
    }
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, wrapMethod);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, wrapMethod);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, wrapMethod);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, filterMin);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, filterMax);
    if(filterMin ==  GL_LINEAR_MIPMAP_LINEAR)
        glGenerateMipmap(GL_TEXTURE_CUBE_MAP);
    
    this->id = textureID;
}

#endif