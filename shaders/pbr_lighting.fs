#version 450 core
out vec4 FragColor;
in vec2 TexCoords;

// our parameters
uniform sampler2D gPosition;
uniform sampler2D gNormal;
uniform sampler2D gAlbedo;
uniform sampler2D gReflection;

uniform samplerCube irradianceMap;
uniform samplerCube depthMap[3];
float ShadowCalculation(vec3 fragPos);

// lights
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

uniform vec3 viewPos;

const float PI = 3.14159265359;
// array of offset direction for sampling
vec3 gridSamplingDisk[20] = vec3[]
(
   vec3(1, 1,  1), vec3( 1, -1,  1), vec3(-1, -1,  1), vec3(-1, 1,  1), 
   vec3(1, 1, -1), vec3( 1, -1, -1), vec3(-1, -1, -1), vec3(-1, 1, -1),
   vec3(1, 1,  0), vec3( 1, -1,  0), vec3(-1, -1,  0), vec3(-1, 1,  0),
   vec3(1, 0,  1), vec3(-1,  0,  1), vec3( 1,  0, -1), vec3(-1, 0, -1),
   vec3(0, 1,  1), vec3( 0, -1,  1), vec3( 0, -1, -1), vec3( 0, 1, -1)
);
  
float DistributionGGX(vec3 N, vec3 H, float roughness);
float GeometrySchlickGGX(float NdotV, float roughness);
float GeometrySmith(vec3 N, vec3 V, vec3 L, float roughness);
vec3 fresnelSchlick(float cosTheta, vec3 F0);

void main()
{	
    float type = texture(gAlbedo, TexCoords).a;
    if(type <= 0.2 || type > 0.4)
        discard;

    vec3 albedo = texture(gAlbedo, TexCoords).rgb;	
    float metallic = texture(gReflection, TexCoords).g ;	
    float roughness = texture(gReflection, TexCoords).b;	
    vec3 N = normalize(texture(gNormal, TexCoords).rgb);
    vec3 worldPos = texture(gPosition, TexCoords).rgb;
    float shadow = ShadowCalculation(worldPos);

    vec3 V = normalize(viewPos - worldPos);

    vec3 F0 = vec3(0.04); 
    F0 = mix(F0, albedo, metallic);
	           
    // reflectance equation
    vec3 Lo = vec3(0.0);
    for(int i = 0; i < lightsNumber; ++i) 
    {
        // calculate per-light radiance
        vec3 L = normalize(lights[i].Position - worldPos);
        vec3 H = normalize(V + L);
        float distance    = length(lights[i].Position - worldPos);
        float attenuation = 1.0 / (distance * distance * lights[i].Quadratic + distance * lights[i].Linear + lights[i].Constant);
        vec3 radiance     = lights[i].Color * attenuation;        
        
        // cook-torrance brdf
        float NDF = DistributionGGX(N, H, roughness);        
        float G   = GeometrySmith(N, V, L, roughness);      
        vec3 F    = fresnelSchlick(max(dot(H, V), 0.0), F0);       
        
        vec3 kS = F;
        vec3 kD = vec3(1.0) - kS;
        kD *= 1.0 - metallic;	  
        
        vec3 numerator    = NDF * G * F;
        float denominator = 4.0 * max(dot(N, V), 0.0) * max(dot(N, L), 0.0) + 0.0001;
        vec3 specular     = numerator / denominator;  
            
        // add to outgoing radiance Lo
        float NdotL = max(dot(N, L), 0.0);                
        Lo += (kD * albedo / PI + specular) * radiance * NdotL; 
    }   

    vec3 kS = fresnelSchlick(max(dot(N, V), 0.0), F0);
    vec3 kD = 1.0 - kS;
    kD *= 1.0 - metallic;	
  
    vec3 irradiance = texture(irradianceMap, N).rgb;
    vec3 diffuse      = irradiance * albedo  * (1.0 - shadow);
    vec3 ambient = (kD * diffuse);
    //vec3 ambient = vec3(0.03) * albedo;
    vec3 color = ambient + Lo *(1.0 - shadow) ;
	

    vec3 result = vec3(1.0) - exp(-color * 2);

    FragColor = vec4(result, 1.0);
}  

float DistributionGGX(vec3 N, vec3 H, float roughness)
{
    float a = roughness*roughness;
    float a2 = a*a;
    float NdotH = max(dot(N, H), 0.0);
    float NdotH2 = NdotH*NdotH;

    float nom   = a2;
    float denom = (NdotH2 * (a2 - 1.0) + 1.0);
    denom = PI * denom * denom;

    return nom / denom;
}
// ----------------------------------------------------------------------------
float GeometrySchlickGGX(float NdotV, float roughness)
{
    float r = (roughness + 1.0);
    float k = (r*r) / 8.0;

    float nom   = NdotV;
    float denom = NdotV * (1.0 - k) + k;

    return nom / denom;
}
// ----------------------------------------------------------------------------
float GeometrySmith(vec3 N, vec3 V, vec3 L, float roughness)
{
    float NdotV = max(dot(N, V), 0.0);
    float NdotL = max(dot(N, L), 0.0);
    float ggx2 = GeometrySchlickGGX(NdotV, roughness);
    float ggx1 = GeometrySchlickGGX(NdotL, roughness);

    return ggx1 * ggx2;
}
// ----------------------------------------------------------------------------
vec3 fresnelSchlick(float cosTheta, vec3 F0)
{
    return F0 + (1.0 - F0) * pow(clamp(1.0 - cosTheta, 0.0, 1.0), 5.0);
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
    float diskRadius = (1.0 + (viewDistance / 50.0)) / 50.0;

    if(lightsNumber < 1)
        return shadow;

    vec3 fragToLight0 = fragPos - lights[0].Position;
    float currentDepth0 = length(fragToLight0);
    for(int j = 0; j < samples; ++j)
    {
        float closestDepth = texture(depthMap[0], vec3(fragToLight0 + gridSamplingDisk[j] * diskRadius)).r;
        closestDepth *= 50.0;   // undo mapping [0;1]
        if(currentDepth0 - bias > closestDepth)
            shadow += 1.0;
    }

    if(lightsNumber < 2)
        return shadow / float(samples);

    vec3 fragToLight1 = fragPos - lights[1].Position;
    float currentDepth1 = length(fragToLight1);
    for(int j = 0; j < samples; ++j)
    {
        float closestDepth = texture(depthMap[1], vec3(fragToLight1 + gridSamplingDisk[j] * diskRadius)).r;
        closestDepth *= 50.0;   // undo mapping [0;1]
        if(currentDepth1 - bias > closestDepth)
            shadow += 1.0;
    }

    if(lightsNumber < 3)
        return (shadow / float(samples * 2));

    //vec3 fragToLight2 = fragPos - lights[2].Position;
    //float currentDepth2 = length(fragToLight2);
    //for(int j = 0; j < samples; ++j)
    //{
    //    float closestDepth = texture(depthMap[2], vec3(fragToLight2 + gridSamplingDisk[j] * diskRadius)).r;
    //    closestDepth *= 25.0;   // undo mapping [0;1]
    //    if(currentDepth2 - bias > closestDepth)
    //        shadow += 1.0;
    //}
    shadow /= float(samples);
        
    // display closestDepth as debug (to visualize depth cubemap)
    //FragColor = vec4(vec3(closestDepth / far_plane), 1.0);    
        
    return shadow;
}