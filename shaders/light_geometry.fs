#version 420 core

layout (location = 0) out vec3 gPosition;
layout (location = 1) out vec3 gNormal;
layout (location = 2) out vec4 gAlbedo;
layout (location = 3) out vec4 gReflection;

uniform vec3 color;
uniform float type;

void main()
{             
    gAlbedo.rgb = color;
    gAlbedo.a = type;
}  