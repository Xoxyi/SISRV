#version 330 core

in VS_OUT {
    vec3 FragPos;
    vec3 Normal;
    vec2 TexCoords;
} fs_in;

out vec4 FragColor;

void main()
{           
    vec3 color = vec3(.5,.2,.3);
    vec3 normal = normalize(fs_in.Normal);
    // ambient
    vec3 ambient = 0.1 * color;
    // lighting
    vec3 lighting = vec3(0.0);
    vec3 lightPos = vec3(5,5,5);

    vec3 lightDir = normalize(lightPos - fs_in.FragPos);
    float diff = max(dot(lightDir, normal), 0.0);
    vec3 diffuse = diff * color;      
    vec3 result = diffuse;        
    
    lighting += result;

    FragColor = vec4(ambient + lighting, 1.0);
}