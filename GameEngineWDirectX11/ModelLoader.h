#pragma once
#include <assimp\scene.h>
#include <string>
#include <vector>
#include "MeshData.h"
#include "Vertex.h"

namespace Luna {
using namespace std;
using namespace DirectX::SimpleMath;
using namespace DirectX;

struct BoneInfo {
    Matrix m_offsetMatrix;
    Matrix m_finalTransformation;
};

struct VertexBoneData {

};

struct BoneNode {

};

class ModelLoader {
  public:
    void Load(std::string basePath, std::string filename);

    void ProcessNode(aiNode *node, const aiScene *scene, Matrix parentTransform);
    std::string ReadFilename(aiMaterial *material, aiTextureType type);
    MeshData ProcessMesh(aiMesh *mesh, const aiScene *scene);
    void ProcessBones(aiMesh *mesh, class Model &model,
                      std::vector<VertexBoneData> &boneData) {}
    
    void TransformVertices(MeshData& mesh, const Matrix& transform);
    void UpdateTangent();

    std::string m_basePath;
    std::vector<MeshData> m_meshes;

  private:
    Matrix AiToMatrix(const aiMatrix4x4 &m);
};
} // namespace Luna