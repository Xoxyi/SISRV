
#version 420 core
out vec4 FragColor;

in vec2 TexCoords;

uniform sampler2D gPosition;
uniform sampler2D gNormal;
uniform sampler2D gAlbedo;
uniform sampler2D gReflection;

uniform samplerCube irradianceMap;
uniform samplerCube depthMap[20];


uniform vec3 viewPos;
float ShadowCalculation(vec3 fragPos);

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

vec3 gridSamplingDisk[20] = vec3[]
(
   vec3(1, 1,  1), vec3( 1, -1,  1), vec3(-1, -1,  1), vec3(-1, 1,  1), 
   vec3(1, 1, -1), vec3( 1, -1, -1), vec3(-1, -1, -1), vec3(-1, 1, -1),
   vec3(1, 1,  0), vec3( 1, -1,  0), vec3(-1, -1,  0), vec3(-1, 1,  0),
   vec3(1, 0,  1), vec3(-1,  0,  1), vec3( 1,  0, -1), vec3(-1, 0, -1),
   vec3(0, 1,  1), vec3( 0, -1,  1), vec3( 0, -1, -1), vec3( 0, 1, -1)
);

void main()
{  
    // retrieve data from gbuffer
    float type = texture(gAlbedo, TexCoords).a;

    if(type >= 0.2){
        discard;
    }

    vec3 FragPos = texture(gPosition, TexCoords).rgb;
    vec3 Normal = normalize(texture(gNormal, TexCoords).rgb);

    vec3 Diffuse = texture(gAlbedo, TexCoords).rgb;
    float Specular = texture(gReflection, TexCoords).r;
    float shadow = ShadowCalculation(FragPos);
    
    // then calculate lighting as usual
    vec3 lighting  = Diffuse * 0.0; // hard-coded ambient component
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

    //vec3 kS = fresnelSchlick(max(dot(N, V), 0.0), F0);
  
    vec3 irradiance = texture(irradianceMap, Normal).rgb;
    vec3 diffuse      = irradiance * Diffuse;
    vec3 ambient = (1 * diffuse);

    lighting = ambient  + lighting * (1.0 - shadow);

    vec3 result = vec3(1.0) - exp(-lighting * .7);
    FragColor = vec4(result, 1.0);
}

float ShadowCalculation(vec3 fragPos)
{
    // get vector between fragment position and light position
    // use the fragment to light vector to sample from the depth map    
    // float closestDepth = texture(depthMap, fragToLight).r;
    // it is currently in linear range between [0,1], let's re-transform it back to original depth value
    // closestDepth *= far_plane;
    // now get current linear depth as the length between the fragment and light position
    // test for shadows
    // float bias = 0.05; // we use a much larger bias since depth is now in [near_plane, far_plane] range
    // float shadow = currentDepth -  bias > closestDepth ? 1.0 : 0.0;
    // PCF
    // float shadow = 0.0;
    // float bias = 0.05; 
    // float samples = 4.0;
    // float offset = 0.1;
    // for(float x = -offset; x < offset; x += offset / (samples * 0.5))
    // {
        // for(float y = -offset; y < offset; y += offset / (samples * 0.5))
        // {
            // for(float z = -offset; z < offset; z += offset / (samples * 0.5))
            // {
                // float closestDepth = texture(depthMap, fragToLight + vec3(x, y, z)).r; // use lightdir to lookup cubemap
                // closestDepth *= far_plane;   // Undo mapping [0;1]
                // if(currentDepth - bias > closestDepth)
                    // shadow += 1.0;
            // }
        // }
    // }
    // shadow /= (samples * samples * samples);
    float shadow = 0.0;
    float bias = 0.15;
    int samples = 20;
    float viewDistance = length(viewPos - fragPos);
    float diskRadius = (1.0 + (viewDistance / 25.0)) / 25.0;
    for(int i =0; i < lightsNumber; i++){
        vec3 fragToLight = fragPos - lights[i].Position;
        float currentDepth = length(fragToLight);
        for(int j = 0; j < samples; ++j)
        {
            float closestDepth = texture(depthMap[0], vec3(fragToLight + gridSamplingDisk[j] * diskRadius)).r;
            closestDepth *= 25.0;   // undo mapping [0;1]
            if(currentDepth - bias > closestDepth)
                shadow += 1.0;
        }
    }
    shadow /= float(samples * lightsNumber);
        
    // display closestDepth as debug (to visualize depth cubemap)
     //FragColor = vec4(vec3(closestDepth / far_plane), 1.0);    
        
    return shadow;
}



