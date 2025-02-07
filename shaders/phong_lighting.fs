
#version 420 core
out vec4 FragColor;

in vec2 TexCoords;

uniform sampler2D gPosition;
uniform sampler2D gNormal;
uniform sampler2D gAlbedo;
uniform sampler2D gReflection;

uniform vec3 viewPos;

struct Light {          // base alignment   // aligned offset
    vec3 Position;      // 16               // 0
    vec3 Color;         // 16               // 16
    float Constant;     // 4                // 32 
    float Linear;       // 4                // 36
    float Quadratic;    // 4                // 40    
    //padding           // 4                // 44
};

layout (std140, binding = 2) uniform Lights{

    Light lights[20];   // 960              // 0
    int lightsNumber;   // 4                // 960
};

void main()
{  
    // retrieve data from gbuffer
    float type = texture(gAlbedo, TexCoords).a;

    if(type >= 0.2){
        discard;
    }

    vec3 FragPos = texture(gPosition, TexCoords).rgb;
    vec3 Normal = texture(gNormal, TexCoords).rgb;
    vec3 Diffuse = texture(gAlbedo, TexCoords).rgb;
    float Specular = texture(gReflection, TexCoords).r;
    
    // then calculate lighting as usual
    vec3 lighting  = Diffuse * 0.1; // hard-coded ambient component
    vec3 viewDir  = normalize(viewPos - FragPos);
    for(int i = 0; i < lightsNumber; ++i)
    {
        // calculate distance between light source and current fragment
        float distance = length(lights[i].Position - FragPos);
        // diffuse
        vec3 lightDir = normalize(lights[i].Position - FragPos);
        vec3 diffuse = max(dot(Normal, lightDir), 0.0) * Diffuse * lights[i].Color;
        // specular
        vec3 halfwayDir = normalize(lightDir + viewDir);  
        float spec = pow(max(dot(Normal, halfwayDir), 0.0), 16.0);
        vec3 specular = lights[i].Color * spec * Specular;
        // attenuation
        float attenuation = 1.0 / (1.0 + lights[i].Linear * distance + lights[i].Quadratic * distance * distance);
        diffuse *= attenuation;
        specular *= attenuation;
        lighting += diffuse + specular;
    }
   FragColor = vec4(lighting, 1.0);
}

