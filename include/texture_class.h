#ifndef TEXTURE_H
#define TEXTURE_H

#include "glad/glad.h"

#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"

#include "stb_image.h"

#include <iostream>
#include <string>





class Texture {
public:

    unsigned int id;
    std::string type;
    std::string path;

    Texture(const char *path, const std::string &directory, bool gammaCorrection, int wrapMethod = GL_REPEAT);

    Texture(const char *path, const std::string &directory, std::string type, bool gammaCorrection, int wrapMethod = GL_REPEAT);
};

Texture::Texture(const char *path, const std::string &directory, bool gammaCorrection, int wrapMethod)
{
    std::string filename = std::string(path);
    filename = directory + '/' + filename;

    unsigned int textureID;
    glGenTextures(1, &textureID);

    int width, height, nrComponents;
    unsigned char *data = stbi_load(filename.c_str(), &width, &height, &nrComponents, 0);
    if (data) {
        GLenum format;
        GLenum internalFormat;

        if (nrComponents == 1) {
            format = GL_RED;
            internalFormat = GL_RED;
        }
        else if (nrComponents == 3) {
            format = GL_RGB;
            internalFormat = gammaCorrection? GL_SRGB : GL_RGB;
        }
        else if (nrComponents == 4) {
            format = GL_RGBA;
            internalFormat = gammaCorrection? GL_SRGB_ALPHA : GL_RGBA;

        }
        else
            format = -1; //error
            internalFormat = -1; //error


        glBindTexture(GL_TEXTURE_2D, textureID);
        glTexImage2D(GL_TEXTURE_2D, 0, internalFormat, width, height, 0, format, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, wrapMethod);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, wrapMethod);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        stbi_image_free(data);
    }
    else {
        std::cerr << "Texture failed to load at path: " << path << std::endl;
        stbi_image_free(data);
    }

    this->id = textureID;
    this->path = path;
}

Texture::Texture(const char *path, const std::string &directory, std::string type, bool gammaCorrection, int wrapMethod) : type(type)
{
    Texture(path, directory, gammaCorrection, wrapMethod);
}

#endif