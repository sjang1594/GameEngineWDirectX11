#pragma once

#pragma once

#include <string>
#include <vector>
#include "Vertex.h"

namespace Luna {

using std::vector;

struct MeshData {
    std::vector<Vertex> vertices;
    std::vector<uint32_t> indices;
    std::string textureFilename;
};

} // namespace Luna