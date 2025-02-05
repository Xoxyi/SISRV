#ifndef MESH_H
#define MESH_H

#include "glad/glad.h" // holds all OpenGL type declarations

#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"

#include "shader_class.h"
#include "texture_class.h"

#include <string>
#include <vector>

const int MAX_BONE_INFLUENCE = 10;

struct Vertex {
    // position
    glm::vec3 Position;
    // normal
    glm::vec3 Normal;
    // texCoords
    glm::vec2 TexCoords;
    // tangent
    glm::vec3 Tangent;
    // bitangent
    glm::vec3 Bitangent;
	//bone indexes which will influence this vertex
	int m_BoneIDs[MAX_BONE_INFLUENCE];
	//weights from each bone
	float m_Weights[MAX_BONE_INFLUENCE];
};

class Mesh {
public:

    unsigned int VAO, VBO, EBO;
    std::vector<unsigned int> indices;
    Mesh(std::vector<Vertex> vertices, std::vector<unsigned int> indices, std::vector<Texture> textures);

    void Draw(Shader& shader);

    static Mesh GenCube(std::vector<Texture> textures);

    static Mesh GenQuad(std::vector<Texture> textures);

    static Mesh GenSphere(std::vector<Texture> textures);

private:

    // mesh data
    std::vector<Vertex> vertices;
    std::vector<Texture> textures;

    //  render data

    void setupMesh();
};

Mesh::Mesh(std::vector<Vertex> vertices, std::vector<unsigned int> indices, std::vector<Texture> textures)
{
    this->vertices = vertices;
    this->indices = indices;
    this->textures = textures;

    setupMesh();
}

void Mesh::setupMesh()
{
    // create buffers/arrays
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);

    glBindVertexArray(VAO);
    // load data into vertex buffers
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    // A great thing about structs is that their memory layout is sequential for all its items.
    // The effect is that we can simply pass a pointer to the struct and it translates perfectly to a glm::vec3/2 array which
    // again translates to 3/2 floats which translates to a byte array.
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), &vertices[0], GL_STATIC_DRAW);  

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), &indices[0], GL_STATIC_DRAW);

    // set the vertex attribute pointers
    // vertex Positions
    glEnableVertexAttribArray(0);	
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);
    // vertex normals
    glEnableVertexAttribArray(1);	
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, Normal));
    // vertex texture coords
    glEnableVertexAttribArray(2);	
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, TexCoords));
    
    // vertex tangent
    glEnableVertexAttribArray(3);
    glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, Tangent));
    // vertex bitangent
    glEnableVertexAttribArray(4);
    glVertexAttribPointer(4, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, Bitangent));
    // ids
    glEnableVertexAttribArray(5);
    glVertexAttribIPointer(5, 4, GL_INT, sizeof(Vertex), (void*)offsetof(Vertex, m_BoneIDs));

    // weights
    glEnableVertexAttribArray(6);
    glVertexAttribPointer(6, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, m_Weights));
    
    glBindVertexArray(0);
}

void Mesh::Draw(Shader &shader) 
{
    // bind appropriate textures
    unsigned int diffuseNr   = 1;
    unsigned int specularNr  = 1;
    unsigned int normalNr    = 1;
    unsigned int heightNr    = 1;
    unsigned int metalnessNr = 1;
    unsigned int roughnessNr = 1;
    for(unsigned int i = 0; i < textures.size(); i++) {
        glActiveTexture(GL_TEXTURE0 + i); // active proper texture unit before binding
        // retrieve texture number (the N in diffuse_textureN)
        std::string number;
        std::string name = textures[i].type;
        if(name == "texture_diffuse")
            number = std::to_string(diffuseNr++);
        else if(name == "texture_specular")
            number = std::to_string(specularNr++); // transfer unsigned int to string
        else if(name == "texture_normal")
            number = std::to_string(normalNr++); // transfer unsigned int to string
        else if(name == "texture_height")
            number = std::to_string(heightNr++); // transfer unsigned int to string
        else if(name == "texture_metalness")
            number = std::to_string(metalnessNr++);
        else if(name == "texture_roughness")
            number = std::to_string(roughnessNr++);


         // now set the sampler to the correct texture unit
         glUniform1i(glGetUniformLocation(shader.ID, (name + number).c_str()), i);
         // and finally bind the texture
         glBindTexture(GL_TEXTURE_2D, textures[i].id);
    }
    // draw mesh
    glBindVertexArray(VAO);
    glDrawElements(GL_TRIANGLES, static_cast<unsigned int>(indices.size()), GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);

    // always good practice to set everything back to defaults once configured.
    glActiveTexture(GL_TEXTURE0);
}

Mesh Mesh::GenCube(std::vector<Texture> textures)
{
    std::vector<Vertex> vertices = {
        // Back face
        {glm::vec3(-1.0f, -1.0f, -1.0f), glm::vec3(0.0f, 0.0f, -1.0f), glm::vec2(0.0f, 0.0f)},
        {glm::vec3(1.0f, 1.0f, -1.0f), glm::vec3(0.0f, 0.0f, -1.0f), glm::vec2(1.0f, 1.0f)},
        {glm::vec3(1.0f, -1.0f, -1.0f), glm::vec3(0.0f, 0.0f, -1.0f), glm::vec2(1.0f, 0.0f)},
        {glm::vec3(1.0f, 1.0f, -1.0f), glm::vec3(0.0f, 0.0f, -1.0f), glm::vec2(1.0f, 1.0f)},
        {glm::vec3(-1.0f, -1.0f, -1.0f), glm::vec3(0.0f, 0.0f, -1.0f), glm::vec2(0.0f, 0.0f)},
        {glm::vec3(-1.0f, 1.0f, -1.0f), glm::vec3(0.0f, 0.0f, -1.0f), glm::vec2(0.0f, 1.0f)},

        // Front face
        {glm::vec3(-1.0f, -1.0f, 1.0f), glm::vec3(0.0f, 0.0f, 1.0f), glm::vec2(0.0f, 0.0f)},
        {glm::vec3(1.0f, -1.0f, 1.0f), glm::vec3(0.0f, 0.0f, 1.0f), glm::vec2(1.0f, 0.0f)},
        {glm::vec3(1.0f, 1.0f, 1.0f), glm::vec3(0.0f, 0.0f, 1.0f), glm::vec2(1.0f, 1.0f)},
        {glm::vec3(1.0f, 1.0f, 1.0f), glm::vec3(0.0f, 0.0f, 1.0f), glm::vec2(1.0f, 1.0f)},
        {glm::vec3(-1.0f, 1.0f, 1.0f), glm::vec3(0.0f, 0.0f, 1.0f), glm::vec2(0.0f, 1.0f)},
        {glm::vec3(-1.0f, -1.0f, 1.0f), glm::vec3(0.0f, 0.0f, 1.0f), glm::vec2(0.0f, 0.0f)},

        // Left face
        {glm::vec3(-1.0f, 1.0f, 1.0f), glm::vec3(-1.0f, 0.0f, 0.0f), glm::vec2(1.0f, 0.0f)},
        {glm::vec3(-1.0f, 1.0f, -1.0f), glm::vec3(-1.0f, 0.0f, 0.0f), glm::vec2(1.0f, 1.0f)},
        {glm::vec3(-1.0f, -1.0f, -1.0f), glm::vec3(-1.0f, 0.0f, 0.0f), glm::vec2(0.0f, 1.0f)},
        {glm::vec3(-1.0f, -1.0f, -1.0f), glm::vec3(-1.0f, 0.0f, 0.0f), glm::vec2(0.0f, 1.0f)},
        {glm::vec3(-1.0f, -1.0f, 1.0f), glm::vec3(-1.0f, 0.0f, 0.0f), glm::vec2(0.0f, 0.0f)},
        {glm::vec3(-1.0f, 1.0f, 1.0f), glm::vec3(-1.0f, 0.0f, 0.0f), glm::vec2(1.0f, 0.0f)},

        // Right face
        {glm::vec3(1.0f, 1.0f, 1.0f), glm::vec3(1.0f, 0.0f, 0.0f), glm::vec2(1.0f, 0.0f)},
        {glm::vec3(1.0f, -1.0f, -1.0f), glm::vec3(1.0f, 0.0f, 0.0f), glm::vec2(0.0f, 1.0f)},
        {glm::vec3(1.0f, 1.0f, -1.0f), glm::vec3(1.0f, 0.0f, 0.0f), glm::vec2(1.0f, 1.0f)},
        {glm::vec3(1.0f, -1.0f, -1.0f), glm::vec3(1.0f, 0.0f, 0.0f), glm::vec2(0.0f, 1.0f)},
        {glm::vec3(1.0f, 1.0f, 1.0f), glm::vec3(1.0f, 0.0f, 0.0f), glm::vec2(1.0f, 0.0f)},
        {glm::vec3(1.0f, -1.0f, 1.0f), glm::vec3(1.0f, 0.0f, 0.0f), glm::vec2(0.0f, 0.0f)},

        // Bottom face
        {glm::vec3(-1.0f, -1.0f, -1.0f), glm::vec3(0.0f, -1.0f, 0.0f), glm::vec2(0.0f, 1.0f)},
        {glm::vec3(1.0f, -1.0f, -1.0f), glm::vec3(0.0f, -1.0f, 0.0f), glm::vec2(1.0f, 1.0f)},
        {glm::vec3(1.0f, -1.0f, 1.0f), glm::vec3(0.0f, -1.0f, 0.0f), glm::vec2(1.0f, 0.0f)},
        {glm::vec3(1.0f, -1.0f, 1.0f), glm::vec3(0.0f, -1.0f, 0.0f), glm::vec2(1.0f, 0.0f)},
        {glm::vec3(-1.0f, -1.0f, 1.0f), glm::vec3(0.0f, -1.0f, 0.0f), glm::vec2(0.0f, 0.0f)},
        {glm::vec3(-1.0f, -1.0f, -1.0f), glm::vec3(0.0f, -1.0f, 0.0f), glm::vec2(0.0f, 1.0f)},

        // Top face
        {glm::vec3(-1.0f, 1.0f, -1.0f), glm::vec3(0.0f, 1.0f, 0.0f), glm::vec2(0.0f, 1.0f)},
        {glm::vec3(1.0f, 1.0f, 1.0f), glm::vec3(0.0f, 1.0f, 0.0f), glm::vec2(1.0f, 0.0f)},
        {glm::vec3(1.0f, 1.0f, -1.0f), glm::vec3(0.0f, 1.0f, 0.0f), glm::vec2(1.0f, 1.0f)},
        {glm::vec3(1.0f, 1.0f, 1.0f), glm::vec3(0.0f, 1.0f, 0.0f), glm::vec2(1.0f, 0.0f)},
        {glm::vec3(-1.0f, 1.0f, -1.0f), glm::vec3(0.0f, 1.0f, 0.0f), glm::vec2(0.0f, 1.0f)},
        {glm::vec3(-1.0f, 1.0f, 1.0f), glm::vec3(0.0f, 1.0f, 0.0f), glm::vec2(0.0f, 0.0f)}
    };

    std::vector<unsigned int> indices = {
        0, 1, 2,    // Back face - Triangle 1
        3, 4, 5,    // Back face - Triangle 2

        6, 7, 8,    // Front face - Triangle 1
        9, 10, 11,  // Front face - Triangle 2

        12, 13, 14, // Left face - Triangle 1
        15, 16, 17, // Left face - Triangle 2

        18, 19, 20, // Right face - Triangle 1
        21, 22, 23, // Right face - Triangle 2

        24, 25, 26, // Bottom face - Triangle 1
        27, 28, 29, // Bottom face - Triangle 2

        30, 31, 32, // Top face - Triangle 1
        33, 34, 35  // Top face - Triangle 2
    };

    return Mesh(vertices, indices, textures);
}

Mesh Mesh::GenQuad(std::vector<Texture> textures)
{
    std::vector<Vertex> vertices = {
        // Front face
        {glm::vec3(-1.0f, 1.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f), glm::vec2(0.0f, 1.0f)},
        {glm::vec3(-1.0f, -1.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f), glm::vec2(0.0f, 0.0f)},
        {glm::vec3(1.0f, 1.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f), glm::vec2(1.0f, 1.0f)},
        {glm::vec3(1.0f, -1.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f), glm::vec2(1.0f, 0.0f)},
    };

    std::vector<unsigned int> indices = {
        0, 1, 2,    // Back face - Triangle 1
        1, 2, 3,    // Back face - Triangle 2
    };

    return Mesh(vertices, indices, textures);
}

Mesh Mesh::GenSphere(std::vector<Texture> textures)
{
    std::vector<Vertex> vertices;
    std::vector<unsigned int> stripIndices;
    std::vector<unsigned int> indices;

    const unsigned int X_SEGMENTS = 64;
    const unsigned int Y_SEGMENTS = 64;
    const float PI = 3.14159265359f;
    for (unsigned int x = 0; x <= X_SEGMENTS; ++x)
    {
        for (unsigned int y = 0; y <= Y_SEGMENTS; ++y)
        {
            float xSegment = (float)x / (float)X_SEGMENTS;
            float ySegment = (float)y / (float)Y_SEGMENTS;
            float xPos = std::cos(xSegment * 2.0f * PI) * std::sin(ySegment * PI);
            float yPos = std::cos(ySegment * PI);
            float zPos = std::sin(xSegment * 2.0f * PI) * std::sin(ySegment * PI);

            glm::vec3 pos = glm::vec3(xPos, yPos, zPos);
            glm::vec3 normals = glm::vec3(xPos, yPos, zPos);
            glm::vec2 uv = glm::vec2(xSegment, ySegment);

            vertices.push_back(Vertex{pos, normals, uv});
        }
    }

    bool oddRow = false;
    for (unsigned int y = 0; y < Y_SEGMENTS; ++y)
    {
        if (!oddRow) // even rows: y == 0, y == 2; and so on
        {
            for (unsigned int x = 0; x <= X_SEGMENTS; ++x)
            {
                stripIndices.push_back(y * (X_SEGMENTS + 1) + x);
                stripIndices.push_back((y + 1) * (X_SEGMENTS + 1) + x);
            }
        }
        else
        {
            for (int x = X_SEGMENTS; x >= 0; --x)
            {
                stripIndices.push_back((y + 1) * (X_SEGMENTS + 1) + x);
                stripIndices.push_back(y * (X_SEGMENTS + 1) + x);
            }
        }
        oddRow = !oddRow;
    }

    for (size_t i = 0; i < stripIndices.size() - 2; ++i) {
        if (i % 2 == 0) {
            // Even index: use vertices as is
            indices.push_back(stripIndices[i]);
            indices.push_back(stripIndices[i + 1]);
            indices.push_back(stripIndices[i + 2]);
        } else {
            // Odd index: swap last two vertices to maintain correct winding order
            indices.push_back(stripIndices[i]);
            indices.push_back(stripIndices[i + 2]);
            indices.push_back(stripIndices[i + 1]);
        }
    }

    return Mesh(vertices, indices, textures);
}




#endif
