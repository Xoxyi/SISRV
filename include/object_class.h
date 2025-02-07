#ifndef OBJECT_CLASS_H
#define OBJECT_CLASS_H

#include "glm/ext/matrix_float4x4.hpp"
#include <cstddef>
#include "model_class.h"
#include "camera_class.h"

class Object
{
public:

	Model &model;
	glm::mat4 transform;

	Object(Model &model, glm::mat4 transform);

	void Draw(Shader& shader);

private:

};

Object::Object(Model &model, glm::mat4 transform) : model(model), transform(transform){}

void Object::Draw(Shader& shader)
{
    shader.use();
    shader.setMat4("view", camera.GetViewMatrix());
    shader.setMat4("projection", camera.GetProjectionMatrix());
	shader.setMat4("model", transform);
    model.Draw(shader);
}

#endif // DEBUG



