#pragma once
#include <directxtk/SimpleMath.h>
#include "Vertex.h"

namespace Luna {
struct MeshData {
    std::vector<Vertex> vertices;
    std::vector<uint32_t> indices;

    std::string albedoTextureFilename;
    std::string emissiveTextureFilename;
    std::string normalTextureFilename;
    std::string heightTextureFilename;
    std::string aoTextureFilename; // Ambient Occlusion
    std::string metallicTextureFilename;
    std::string roughnessTextureFilename;
};
} // namespace Luna
