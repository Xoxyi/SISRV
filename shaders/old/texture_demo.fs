#version 330 core

in VS_OUT {
    vec3 FragPos;
    vec3 Normal;
    vec2 TexCoords;
} fs_in;

uniform bool selected;
uniform sampler2D texture_diffuse1;

out vec4 FragColor;

void main()
{           
    vec3 color = vec3(texture(texture_diffuse1, fs_in.TexCoords));
    vec3 normal = normalize(fs_in.Normal);
    // ambient
    vec3 ambient = 0.2 * color;
    // lighting
    vec3 lighting = vec3(0.0);
    vec3 lightPos = vec3(5,5,5);

    vec3 lightDir = normalize(lightPos - fs_in.FragPos);
    float diff = max(dot(lightDir, normal), 0.0);
    vec3 diffuse = diff * color;      
    vec3 result = diffuse;        
    
    lighting += result;

    int mult = selected? 1 : 0;

    FragColor = vec4(ambient + lighting + (mult * vec3(0.2f)), 1.0);
}