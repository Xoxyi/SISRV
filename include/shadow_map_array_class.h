#include <vector>
#include "shadow_map_class.h"
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

    // Create storage for the cube map array
    glTexStorage3D(GL_TEXTURE_CUBE_MAP_ARRAY, 1, GL_DEPTH_COMPONENT, 1024, 1024, lightNumber * 6); // 6 faces per light

    // Iterate over all the shadow maps (cube maps)
    for (int i = 0; i < lightNumber; ++i) {
        unsigned int sourceCubeMap = shadowMaps[i].depthCubeMap.id;  // Get source cube map

        // Iterate over each face of the cube map (0 to 5)
                    // Copy data from the source cube map face to the destination cube map array
            glCopyImageSubData(
                sourceCubeMap, GL_TEXTURE_CUBE_MAP, 0,          // Source texture and level
                0, 0, 0,                                   // Source position (x, y, z) with face index
                ID, GL_TEXTURE_CUBE_MAP_ARRAY, 0,              // Destination cube map array and level
                0, 0, i,                           // Destination position (layer index) — (light * 6 + face)
                1024, 1024, 6                                 // Copy dimensions (1024x1024 for depth, 1 layer)
            );
       
    }
}
#endif
