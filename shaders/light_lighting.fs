#version 420 core
out vec4 FragColor;

in vec2 TexCoords;

uniform sampler2D gAlbedo;
uniform sampler2D gReflection;

void main() {
	float type = texture(gReflection, TexCoords).a;
	if(type < 0.4 || type >= 0.6)
		discard;
	vec3 color = texture(gAlbedo, TexCoords).rgb;
	FragColor = vec4(color, 1.0); 
}