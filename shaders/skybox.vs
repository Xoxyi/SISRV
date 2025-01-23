#version 420 core

layout (location = 0) in vec3 aPos;

out vec3 TexCoords;


layout (std140, binding = 0) uniform Matrices
{
    mat4 projection;
    mat4 view;
};

void main()
{
    mat4 newView = view;
    newView[3] = vec4(0.0);
    newView[0][3] = 0.0;
    newView[1][3] = 0.0;
    newView[2][3] = 0.0;
    TexCoords = aPos;
    vec4 pos = projection * newView * vec4(aPos, 1.0);
    gl_Position = pos.xyww;
}  