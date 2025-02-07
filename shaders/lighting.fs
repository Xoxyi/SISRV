

#version 330 core
out vec4 FragColor;

in vec2 TexCoords;

uniform sampler2D gPosition;

void main()
{   
    vec3 col = texture(gPosition, TexCoords).rgb + vec3(.1,.1,.1);
    FragColor = vec4(col, 1.0);
}

