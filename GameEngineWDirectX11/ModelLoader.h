#pragma once
#include <assimp\scene.h>
#include <string>
#include <vector>

#include "MeshData.h"
#include "Vertex.h"

namespace Luna {
using namespace std;
using namespace DirectX::SimpleMath;

class ModelLoader {
  public:
    void Load(std::string basePath, std::string filename);

    void ProcessNode(aiNode *node, const aiScene *scene, Matrix parentTransform);
    std::string ReadFilename(aiMaterial *material, aiTextureType type);
    MeshData ProcessMesh(aiMesh *mesh, const aiScene *scene);

  public:
    std::string m_basePath;
    std::vector<MeshData> m_meshes;
};
} // namespace Luna