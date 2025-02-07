#ifndef SCENE_CLASS_H
#define SCENE_CLASS_H
#include <vector>
#include "object_class.h"
#include "point_light_class.h"

class Scene {
public:
	std::vector<Object> phongObjects;
	std::vector<Object> pbrObjects;
	std::vector<PointLight> pointLights;

	Scene(std::vector<Object>& phongobjects, std::vector<Object>& pbrObjects, std::vector<PointLight>& pointLights) :
		phongObjects(phongobjects),
		pbrObjects(pbrObjects),
		pointLights(pointLights) {

		unsigned int uboLightBlock;
		glGenBuffers(1, &uboLightBlock);
		glBindBuffer(GL_UNIFORM_BUFFER, uboLightBlock);
		glBufferData(GL_UNIFORM_BUFFER, 964, NULL, GL_STATIC_DRAW); // allocate 152 bytes of memory
		for (int i = 0; i < pointLights.size(); ++i) {
			glBufferSubData(GL_UNIFORM_BUFFER, 48 * i + 0, sizeof(glm::vec3), glm::value_ptr(pointLights[i].position));
			glBufferSubData(GL_UNIFORM_BUFFER, 48 * i + 16, sizeof(glm::vec3), glm::value_ptr(pointLights[i].color));
			glBufferSubData(GL_UNIFORM_BUFFER, 48 * i + 32, sizeof(float), &pointLights[i].constant);
			glBufferSubData(GL_UNIFORM_BUFFER, 48 * i + 36, sizeof(float), &pointLights[i].linear);
			glBufferSubData(GL_UNIFORM_BUFFER, 48 * i + 40, sizeof(float), &pointLights[i].quadratic);
		}
		int n = pointLights.size();
		glBufferSubData(GL_UNIFORM_BUFFER, 960, sizeof(int), &n);
		glBindBufferBase(GL_UNIFORM_BUFFER, 2, uboLightBlock);
		glBindBuffer(GL_UNIFORM_BUFFER, 0);
	}

	void DrawPhong(Shader& shader);
	void DrawPbr(Shader& shader);
	void DrawLight(Shader& shader);
};

void Scene::DrawPhong(Shader& shader) {
	for (auto& object : phongObjects) {
		shader.use();
		shader.setFloat("type", .1);
		object.Draw(shader);
	}
}

void Scene::DrawPbr(Shader& shader) {
	for (auto& object : pbrObjects) {
		shader.use();
		shader.setFloat("type", .3);
		object.Draw(shader);
	}
}

void Scene::DrawLight(Shader& shader) {
	for (auto& light : pointLights) {
		shader.use();
		shader.setFloat("type", .5);
		light.Draw(shader);
	}
}

#endif
