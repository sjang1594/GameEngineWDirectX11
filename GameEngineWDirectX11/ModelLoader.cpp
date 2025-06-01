#include "pch.h"
#include "ModelLoader.h"
namespace Luna {
std::string ModelLoader::GetExtension(const std::string filename) {
    std::string extension(fs::path(filename).extension().string());
    transform(extension.begin(), extension.end(), extension.begin(), ::tolower);
    return extension;
}

void ModelLoader::Load(std::string basePath, std::string filename, bool revertNormal) {
    m_basePath = basePath;
    const std::string extension = GetExtension(filename);
    m_isGLTF = (extension == ".gltf");
    m_revertNormals = revertNormal;

    Assimp::Importer importer;
    const std::string fullPath = basePath + filename;
    const aiScene *scene =
        importer.ReadFile(fullPath, aiProcess_Triangulate | aiProcess_ConvertToLeftHanded);

    if (!scene || !scene->mRootNode) {
        std::cerr << "Failed to read file: " << basePath + filename << std::endl;
        return;
    }
    
    Matrix identityMatrix; 
    ProcessNode(scene->mRootNode, scene, identityMatrix);
    UpdateTangents();
}

void ModelLoader::ProcessNode(aiNode *node, const aiScene *scene, Matrix parentTransform) {
    Matrix localTransform(reinterpret_cast <const float *>(&node->mTransformation));
    Matrix worldTransform = localTransform.Transpose() * parentTransform;

    for (UINT i = 0; i < node->mNumMeshes; ++i) {
        aiMesh *mesh = scene->mMeshes[node->mMeshes[i]];
        MeshData newMesh = ProcessMesh(mesh, scene);

        for (auto &vertex : newMesh.vertices) {
            vertex.position = Vector3::Transform(vertex.position, worldTransform);
        }
        m_meshes.emplace_back(std::move(newMesh));
    }

    for (UINT i = 0; i < node->mNumChildren; ++i) {
        ProcessNode(node->mChildren[i], scene, worldTransform);
    }
}

void ModelLoader::LoadMaterialTextures(aiMaterial *material, MeshData &mesh) {
    auto resolve = [&](aiTextureType type) { return ReadFileName(material, type); };

    mesh.albedoTextureFilename = resolve(aiTextureType_BASE_COLOR);
    mesh.aoTextureFilename = resolve(aiTextureType_AMBIENT_OCCLUSION);
    mesh.emissiveTextureFilename = resolve(aiTextureType_EMISSIVE);
    mesh.heightTextureFilename = resolve(aiTextureType_HEIGHT);
    mesh.normalTextureFilename = resolve(aiTextureType_NORMALS);
    mesh.metallicTextureFilename = resolve(aiTextureType_METALNESS);
    mesh.roughnessTextureFilename = resolve(aiTextureType_DIFFUSE_ROUGHNESS);

    if (mesh.aoTextureFilename.empty()) {
        mesh.aoTextureFilename = resolve(aiTextureType_LIGHTMAP);
    }
    if (mesh.albedoTextureFilename.empty()) {
        mesh.albedoTextureFilename = resolve(aiTextureType_DIFFUSE);
    }
}

MeshData ModelLoader::ProcessMesh(aiMesh *mesh, const aiScene *scene) {
    MeshData data;
    data.vertices.reserve(mesh->mNumVertices);
    data.indices.reserve(mesh->mNumFaces * 3);

    for (UINT i = 0; i < mesh->mNumVertices; i++) {
        Vertex v;

        v.position = {mesh->mVertices[i].x, mesh->mVertices[i].y, mesh->mVertices[i].z};

       if (mesh->HasNormals()) {
           const auto &n = mesh->mNormals[i];
           v.normalModel = m_isGLTF ? Vector3(n.x, n.z, -n.y) : Vector3(n.x, n.y, n.z);

           if (m_revertNormals) {
               v.normalModel *= -1.0f;
           }

           v.normalModel.Normalize();
       }

       if (mesh->HasTextureCoords(0)) {
           v.texcoord = {static_cast<float>(mesh->mTextureCoords[0][i].x),
                         static_cast<float>(mesh->mTextureCoords[0][i].y)
           };
       }

       data.vertices.emplace_back(std::move(v));
    }

    for (unsigned int i = 0; i < mesh->mNumFaces; ++i) {
        const aiFace &face = mesh->mFaces[i];
        for (unsigned int j = 0; j < face.mNumIndices; ++j) {
            data.indices.push_back(face.mIndices[j]);
        }
    }

    if (mesh->mMaterialIndex >= 0) {
        aiMaterial *material = scene->mMaterials[mesh->mMaterialIndex];
        LoadMaterialTextures(material, data);
    }
    return data;
}

std::string ModelLoader::ReadFileName(aiMaterial *material, aiTextureType type) {
    if (material->GetTextureCount(type) == 0)
        return "";
    aiString path;
    if (material->GetTexture(type, 0, &path) == AI_SUCCESS) {
        return (fs::path(m_basePath) / fs::path(path.C_Str()).filename()).string();
    }
    return "";
}

void ModelLoader::UpdateTangents() {
    // https://github.com/microsoft/DirectXMesh/wiki/ComputeTangentFrame

    for (auto &mesh : m_meshes) {
        size_t count = mesh.vertices.size();
        std::vector<XMFLOAT3> positions(count);
        std::vector<XMFLOAT3> normals(count);
        std::vector<XMFLOAT2> texcoords(count);
        std::vector<XMFLOAT3> tangents(count);
        std::vector<XMFLOAT3> bitangents(count);

        for (size_t i = 0; i < count; ++i) {
            positions[i] = mesh.vertices[i].position;
            normals[i] = mesh.vertices[i].normalModel;
            texcoords[i] = mesh.vertices[i].texcoord;
        }

        DirectX::ComputeTangentFrame(mesh.indices.data(), mesh.indices.size() / 3, positions.data(),
                                     normals.data(), texcoords.data(), count, tangents.data(),
                                     bitangents.data());

        for (size_t i = 0; i < count; ++i) {
            mesh.vertices[i].tangentModel = tangents[i];
        }
    }
}
} // namespace Luna