
#version 330 core
out vec4 FragColor;

in vec2 TexCoords;

uniform sampler2D gPosition;
uniform sampler2D gNormal;
uniform sampler2D gAlbedo;
uniform sampler2D gReflection;

uniform vec3 viewPos;

void main()
{  
    float type = texture(gReflection, TexCoords).a;
    if( type <= 0.2)
        discard;

    vec3 Normal = texture(gNormal, TexCoords).rgb;
    FragColor = vec4(1.0, 0.0, 0.0 , 1.0);
}

