#version 450 core
layout (location = 0) out vec3 gPosition;
layout (location = 1) out vec3 gNormal;
layout (location = 2) out vec4 gAlbedo;
layout (location = 3) out vec4 gReflection;

in VS_OUT {
    vec3 FragPos;
    vec3 Normal;
    vec2 TexCoords;
    mat3 TBN;
} fs_in;

// material parameters
uniform sampler2D texture_diffuse1;
uniform sampler2D texture_normal1;
uniform sampler2D texture_metallic1;
uniform sampler2D texture_roughness1;

uniform float type;

void main()
{    
    // store the fragment position vector in the first gbuffer texture
    gPosition = fs_in.FragPos;
    // also store the per-fragment normals into the gbuffer
    vec3 normal = texture(texture_normal1, fs_in.TexCoords).rgb;
    normal = normal * 2.0 - 1.0;   
    normal = normalize(fs_in.TBN * normal); 
    gNormal = normal;
    // and the diffuse per-fragment color
    gAlbedo.rgb = texture(texture_diffuse1, fs_in.TexCoords).rgb;
    // store specular intensity in gAlbedoSpec's alpha component
    gReflection.g = texture(texture_metallic1, fs_in.TexCoords).r;
    gReflection.b = texture(texture_roughness1, fs_in.TexCoords).r;

    gAlbedo.a = type;

}