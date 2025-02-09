#version 450 core
out vec4 FragColor;

in vec2 TexCoords;

uniform sampler2D gAlbedo;

void main() {
	float type = texture(gAlbedo, TexCoords).a;
	if(type < 0.1 || type >= 0.6)
		discard;
	vec3 color = texture(gAlbedo, TexCoords).rgb;

	vec3 result = vec3(1.0) - exp(-color * 1);
	FragColor = vec4(result, 1.0); 
}