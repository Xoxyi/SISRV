#version 420 core
out vec4 FragColor;

in vec2 TexCoords;

uniform sampler2D gAlbedo;

void main() {
	float type = texture(gAlbedo, TexCoords).a;
	if(type < 0.1 || type >= 0.6)
		discard;
	vec3 color = texture(gAlbedo, TexCoords).rgb;
	FragColor = vec4(1.0, 0.0, 0.0, 1.0); 
}