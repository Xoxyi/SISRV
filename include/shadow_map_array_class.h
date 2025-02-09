#include <vector>
#include <shadow_map_class.h>
#ifndef SHADOW_MAP_ARRAY_H
#define SHADOW_MAP_ARRAY_H

class ShadowMapArray {
public:
	unsigned int ID;
	unsigned int lightNumber;
	ShadowMapArray(std::vector<ShadowMap>& shadowMaps);
};

ShadowMapArray::ShadowMapArray(std::vector<ShadowMap>& shadowMaps) : lightNumber(shadowMaps.size()) {
	glGenTextures(1, &ID);
	glBindTexture(GL_TEXTURE_CUBE_MAP_ARRAY, ID);
	glTexStorage3D(GL_TEXTURE_CUBE_MAP_ARRAY, 1, GL_DEPTH_COMPONENT, 1024, 1024, lightNumber * 6);

	for (int i = 0; i < lightNumber; ++i) {
		unsigned int sourceCubeMap = shadowMaps[i].depthCubeMap.id;  // Your existing cube maps

		for (int face = 0; face < 6; ++face) {
			glCopyImageSubData(sourceCubeMap, GL_TEXTURE_CUBE_MAP, 0, 0, 0, face,
				ID, GL_TEXTURE_CUBE_MAP_ARRAY, 0, 0, 0, i * 6 + face,
				1024, 1024, 1);
		}
	}
}

#endif
