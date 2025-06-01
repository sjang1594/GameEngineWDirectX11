#pragma once

#include <DirectXMesh.h>
#include <assimp\Importer.hpp>
#include <assimp\postprocess.h>
#include <assimp\scene.h>
#include <MeshData.h>
#include <string>

namespace Luna {
namespace fs = std::filesystem;
using DirectX::SimpleMath::Matrix;
using DirectX::SimpleMath::Vector3;
using DirectX::XMFLOAT3;
using DirectX::XMFLOAT2;

class ModelLoader {
  public:
    std::string GetExtension(const std::string filename);
    void Load(std::string basePath, std::string filename, bool revertNormal);
    void ProcessNode(aiNode *node, const aiScene *scene, Matrix parentTransform);
    MeshData ProcessMesh(aiMesh *mesh, const aiScene *scene);
    std::string ReadFileName(aiMaterial *material, aiTextureType type);
    void LoadMaterialTextures(aiMaterial *material, MeshData &mesh);
    void UpdateTangents();

    std::string m_basePath;
    std::vector<MeshData> m_meshes;
    bool m_isGLTF = false;
    bool m_revertNormals = false;
};
} // namespace Luna