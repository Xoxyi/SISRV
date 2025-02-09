#ifndef SHADOW_MAP_H
#define SHADOW_MAP_H

#include "point_light_class.h"
#include "cube_map_class.h"
#include "frame_buffer_class.h"
#include "scene_class.h"
#include "camera_class.h"

class ShadowMap {
public:
	CubeMap depthCubeMap;
	PointLight *lightSource;
	FrameBuffer depthMapFBO;
	Shader depthShader;
	float nearPlane;
	float farPlane;
	unsigned int shadowWidth;
	unsigned int shadowHeight;

	ShadowMap(PointLight *light, float shadowWidth = 1024, float shadowHeight = 1024,  float nearPlane = 1.0f, float farPlane = 25.0f);
	void Draw(Scene scene);

};

ShadowMap::ShadowMap(PointLight *light, float shadowWidth, float shadowHeight, float nearPlane, float farPlane) :
	lightSource(light), depthCubeMap(GL_CLAMP_TO_EDGE, shadowWidth, shadowHeight, GL_DEPTH_COMPONENT, GL_DEPTH_COMPONENT, GL_FLOAT),
	shadowWidth(shadowWidth), shadowHeight(shadowHeight),
	nearPlane(nearPlane), farPlane(farPlane),
	depthMapFBO(), depthShader("shaders/point_shadows_depth.vs", "shaders/point_shadows_depth.gs", "shaders/point_shadows_depth.fs") {

	depthMapFBO.enable();
	glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, depthCubeMap.id, 0);
	glDrawBuffer(GL_NONE);
	glReadBuffer(GL_NONE);
	depthMapFBO.disable();
}

void ShadowMap::Draw(Scene scene) {
	glm::mat4 shadowProj = glm::perspective(glm::radians(90.0f), (float)shadowWidth / (float)shadowHeight, nearPlane, farPlane);
	std::vector<glm::mat4> shadowTransforms;
	shadowTransforms.push_back(shadowProj * glm::lookAt(lightSource->position, lightSource->position + glm::vec3(1.0f, 0.0f, 0.0f), glm::vec3(0.0f, -1.0f, 0.0f)));
	shadowTransforms.push_back(shadowProj * glm::lookAt(lightSource->position, lightSource->position + glm::vec3(-1.0f, 0.0f, 0.0f), glm::vec3(0.0f, -1.0f, 0.0f)));
	shadowTransforms.push_back(shadowProj * glm::lookAt(lightSource->position, lightSource->position + glm::vec3(0.0f, 1.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f)));
	shadowTransforms.push_back(shadowProj * glm::lookAt(lightSource->position, lightSource->position + glm::vec3(0.0f, -1.0f, 0.0f), glm::vec3(0.0f, 0.0f, -1.0f)));
	shadowTransforms.push_back(shadowProj * glm::lookAt(lightSource->position, lightSource->position + glm::vec3(0.0f, 0.0f, 1.0f), glm::vec3(0.0f, -1.0f, 0.0f)));
	shadowTransforms.push_back(shadowProj * glm::lookAt(lightSource->position, lightSource->position + glm::vec3(0.0f, 0.0f, -1.0f), glm::vec3(0.0f, -1.0f, 0.0f)));

	glViewport(0, 0, shadowWidth, shadowHeight);
	depthMapFBO.enable();
	glClear(GL_DEPTH_BUFFER_BIT);
	depthShader.use();
	for (unsigned int i = 0; i < 6; ++i)
		depthShader.setMat4("shadowMatrices[" + std::to_string(i) + "]", shadowTransforms[i]);
	depthShader.setFloat("far_plane", farPlane);
	depthShader.setVec3("lightPos", lightSource->position);
	scene.DrawPhong(depthShader);
	scene.DrawPbr(depthShader);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glViewport(0, 0, SCR_WIDTH, SCR_HEIGHT);
}


#endif