#ifndef POINT_LIGHT_CLASS_H
#define POINT_LIGHT_CLASS_H
#include "camera_class.h"
#include "glm/glm.hpp"
#include "model_class.h"
	
class PointLight {

public:
	glm::vec3 position;
	glm::vec3 color;
	float constant;
	float linear;
	float quadratic;

	PointLight(glm::vec3 position, glm::vec3 color, float constant, float linear, float quadratic) : position(position),
													  color(color),
													  constant(constant),
													  linear(linear),
													  quadratic(quadratic){}

	void Draw(Shader& shader) {
		static Model model = Model::GenSphere();
		shader.use();
		shader.setMat4("view", camera.GetViewMatrix());
		shader.setMat4("projection", camera.GetProjectionMatrix());
		shader.setMat4("model", glm::scale(glm::translate(glm::mat4(1.0), position), glm::vec3(0.2, 0.2, 0.2)));
		shader.setVec3("color", color);
		shader.setFloat("type", 0.5);
		model.Draw(shader);
	}

};


#endif