#include "pch.h"
#include "ModelLoader.h"

namespace Luna {
using namespace std;

void ModelLoader::Load(std::string basePath, std::string filename) {
    m_basePath = basePath;

    Assimp::Importer importer;
    std::string fullPath = (std::filesystem::path(basePath) / filename).string();

    const aiScene *pScene = importer.ReadFile(
        fullPath, aiProcess_Triangulate | aiProcess_ConvertToLeftHanded |
                      aiProcess_CalcTangentSpace | aiProcess_GenBoundingBoxes |
                      aiProcess_OptimizeMeshes | aiProcess_GenSmoothNormals |
                      aiProcess_ValidateDataStructure);

    if (!pScene || !pScene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !pScene->mRootNode) {
        std::cerr << "[Model Loader] Failed to load: " << fullPath << "\n";
        std::cerr << "[Assimp] " << importer.GetErrorString() << "\n";
        return;
    }

    Matrix identityM;
    ProcessNode(pScene->mRootNode, pScene, identityM);
}

void ModelLoader::ProcessNode(aiNode *node, const aiScene *scene,
                              Matrix parentTransform) {
    Matrix localTransform = AiToMatrix(node->mTransformation);
    Matrix combinedTransform = localTransform * parentTransform;

    for (UINT i = 0; i < node->mNumMeshes; i++) {
        aiMesh *mesh = scene->mMeshes[node->mMeshes[i]];
        MeshData meshData = ProcessMesh(mesh, scene);
        TransformVertices(meshData, combinedTransform);
        m_meshes.emplace_back(meshData);
    }

    for (UINT i = 0; i < node->mNumChildren; i++) {
        ProcessNode(node->mChildren[i], scene, combinedTransform);
    }
}

std::string ModelLoader::ReadFilename(aiMaterial *material, aiTextureType type) {
    aiString path;

    if (material->GetTextureCount(type) > 0 &&
        material->GetTexture(type, 0, &path) == aiReturn_SUCCESS) {
        return m_basePath +
               std::string(std::filesystem::path(path.C_Str()).filename().string());
    } else {
        return "";
    }
}

MeshData ModelLoader::ProcessMesh(aiMesh *mesh, const aiScene *scene) {
    MeshData data;
    data.name = mesh->mName.C_Str();
    data.vertices.reserve(mesh->mNumVertices);
    data.indices.reserve(mesh->mNumFaces * 3);

    // Walk through each of the mesh's vertices
    for (UINT i = 0; i < mesh->mNumVertices; i++) {
        Vertex v;

        const auto &pos = mesh->mVertices[i];
        v.position = {pos.x, pos.y, pos.z};

        if (mesh->HasNormals()) {
            const auto &normal = mesh->mNormals[i];
            v.normal = {normal.x, normal.y, normal.z};
            v.normal.Normalize();
        }

        if (mesh->HasTextureCoords(0)) {
            const auto &uv = mesh->mTextureCoords[0][i];
            v.texcoord = {uv.x, uv.y};
        }

        if (mesh->HasTangentsAndBitangents()) {
            const auto &tangent = mesh->mTangents[i];
            v.tangent = {tangent.x, tangent.y, tangent.z};
        }

        data.vertices.emplace_back(v);
    }

    // Indices
    for (UINT i = 0; i < mesh->mNumFaces; i++) {
        aiFace face = mesh->mFaces[i];
        data.indices.insert(data.indices.end(), face.mIndices,
                            face.mIndices + face.mNumIndices);
    }

    // Material
    if (mesh->mMaterialIndex <= scene->mNumMaterials) {
        aiMaterial *material = scene->mMaterials[mesh->mMaterialIndex];
        data.albedoTextureFilename = ReadFilename(material, aiTextureType_BASE_COLOR);
        data.emissiveTextureFilename = ReadFilename(material, aiTextureType_EMISSIVE);
        data.heightTextureFilename = ReadFilename(material, aiTextureType_HEIGHT);
        data.normalTextureFilename = ReadFilename(material, aiTextureType_NORMALS);
        data.metallicTextureFilename = ReadFilename(material, aiTextureType_METALNESS);
        data.roughnessTextureFilename =
            ReadFilename(material, aiTextureType_DIFFUSE_ROUGHNESS);
        data.aoTextureFilename = ReadFilename(material, aiTextureType_AMBIENT_OCCLUSION);
    }

    return data;
}

void ModelLoader::TransformVertices(MeshData &mesh, const Matrix &transform) {
    // TODO: Parallel Approach Later
    const Matrix normalMatrix = transform.Invert().Transpose();

    for (auto &vertex : mesh.vertices) {
        vertex.position = Vector3::Transform(vertex.position, transform);

        if (vertex.normal.LengthSquared() > 0.0f) {
            vertex.normal = Vector3::TransformNormal(vertex.normal, normalMatrix);
            vertex.normal.Normalize();
        }

        if (vertex.tangent.LengthSquared() > 0.0f) {
            vertex.tangent = Vector3::TransformNormal(vertex.tangent, normalMatrix);
            vertex.tangent.Normalize();
        }
    }
}

void ModelLoader::UpdateTangent() {
    for (auto &m : m_meshes) {
        vector<XMFLOAT3> positions(m.vertices.size());
        vector<XMFLOAT3> normals(m.vertices.size());
        vector<XMFLOAT2> texcoords(m.vertices.size());
        vector<XMFLOAT3> tangents(m.vertices.size());
        vector<XMFLOAT3> bitangents(m.vertices.size());

        for (size_t i = 0; i < m.vertices.size(); i++) {
            auto &v = m.vertices[i];
            positions[i] = v.position;
            normals[i] = v.normal;
            texcoords[i] = v.texcoord;
        }

        ComputeTangentFrame(m.indices.data(), m.indices.size() / 3, positions.data(),
                            normals.data(), texcoords.data(), m.vertices.size(),
                            tangents.data(), bitangents.data());

        for (size_t i = 0; i < m.vertices.size(); i++) {
            m.vertices[i].tangent = tangents[i];
        }
    }
}

Matrix ModelLoader::AiToMatrix(const aiMatrix4x4 &m) {
    return Matrix(m.a1, m.b1, m.c1, m.d1, m.a2, m.b2, m.c2, m.d2, m.a3, m.b3, m.c3, m.d3,
                  m.a4, m.b4, m.c4, m.d4);
}
} // namespace Luna