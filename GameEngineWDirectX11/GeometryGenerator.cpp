#include "pch.h"
#include "GeometryGenerator.h"
#include "ModelLoader.h"

namespace Luna {
std::vector<MeshData> GeometryGenerator::ReadFromFile(std::string basePath, std::string fileName,
                                                      bool revertNormals) {
    ModelLoader modelLoader;
    modelLoader.Load(basePath, fileName, revertNormals);
    std::vector<MeshData> &meshes = modelLoader.m_meshes;

    constexpr float inf = std::numeric_limits<float>::infinity();
    Vector3 min = Vector3(inf, inf, inf);
    Vector3 max = Vector3(-inf, -inf, -inf);

    for (const auto &mesh : meshes) {
        for (const auto &v : mesh.vertices) {
            min = Vector3::Min(min, v.position);
            max = Vector3::Max(max, v.position);
        }
    }

    Vector3 center = (min + max) * 0.5f;
    Vector3 size = max - min;
    float maxExtent = std::max({size.x, size.y, size.z});

    if (maxExtent < 0.0f) {
        std::cerr << "Failed to read file due to size being small " << std::endl;
    }

    for (auto &mesh : meshes) {
        for (auto &v : mesh.vertices) {
            v.position = (v.position - center) / maxExtent;
        }
    }
    return meshes;
}

MeshData GeometryGenerator::MakeSquare(const float scale, const Vector2 texScale) {
    std::vector<Vector3> positions;
    std::vector<Vector3> colors;
    std::vector<Vector3> normals;
    std::vector<Vector2> texcoords; // texture coordinator

    // Front Face
    positions.emplace_back(Vector3(-1.0f, 1.0f, 0.0f) * scale);
    positions.emplace_back(Vector3(1.0f, 1.0f, 0.0f) * scale);
    positions.emplace_back(Vector3(1.0f, -1.0f, 0.0f) * scale);
    positions.emplace_back(Vector3(-1.0f, -1.0f, 0.0f) * scale);
    colors.emplace_back(Vector3(0.0f, 0.0f, 1.0f));
    colors.emplace_back(Vector3(0.0f, 0.0f, 1.0f));
    colors.emplace_back(Vector3(0.0f, 0.0f, 1.0f));
    colors.emplace_back(Vector3(0.0f, 0.0f, 1.0f));
    normals.emplace_back(Vector3(0.0f, 0.0f, -1.0f));
    normals.emplace_back(Vector3(0.0f, 0.0f, -1.0f));
    normals.emplace_back(Vector3(0.0f, 0.0f, -1.0f));
    normals.emplace_back(Vector3(0.0f, 0.0f, -1.0f));

    // Texture Coordinate
    texcoords.emplace_back(Vector2(0.0f, 0.0f));
    texcoords.emplace_back(Vector2(1.0f, 0.0f));
    texcoords.emplace_back(Vector2(1.0f, 1.0f));
    texcoords.emplace_back(Vector2(0.0f, 1.0f));

    MeshData meshData;

    for (size_t i = 0; i < positions.size(); i++) {
        Vertex v;
        v.position = positions[i];
        v.normalModel = normals[i];
        v.texcoord = texcoords[i] * texScale;
        v.tangentModel = Vector3(1.0f, 0.0f, 0.0f);
        meshData.vertices.emplace_back(v);
    }

    meshData.indices = {
        0, 1, 2, 0, 2, 3,
    };

    return meshData;
}

MeshData GeometryGenerator::MakeSquareGrid(const int numSlices, const int numStacks,
                                           const float scale,
                                       const Vector2 texScale) {
    MeshData meshData;

    float dx = 2.0f / numSlices;
    float dy = 2.0f / numStacks;

    float y = 1.0f;
    for (int i = 0; i < numStacks + 1; i++) {
        float x = -1.0f;

        for (int j = 0; j < numSlices + 1; j++) {
            Vertex v;
            v.position = Vector3(x, y, 0.0f) * scale;
            v.normalModel = Vector3(0.0f, 0.0f, -1.0f);
            v.texcoord = Vector2(x + 1.0f, y + 1.0f) * 0.5f * texScale;
            v.tangentModel = Vector3(1.0f, 0.0f, 0.0f);

            meshData.vertices.emplace_back(v);
            x += dx;
        }
        y -= dy;
    }

    // Set Indices
    for (int i = 0; i < numStacks; i++) {
        for (int j = 0; j < numSlices; j++) {
            meshData.indices.emplace_back((numSlices + 1) * i + j);
            meshData.indices.emplace_back((numSlices + 1) * i + j + 1);
            meshData.indices.emplace_back((numSlices + 1) * (i + 1) + j);
            meshData.indices.emplace_back((numSlices + 1) * (i + 1) + j);
            meshData.indices.emplace_back((numSlices + 1) * i + j + 1);
            meshData.indices.emplace_back((numSlices + 1) * (i + 1) + j + 1);
        }
    }
    return meshData;
}

MeshData GeometryGenerator::MakeCube(const float scale) {
    std::vector<Vector3> positions;
    std::vector<Vector3> colors;
    std::vector<Vector3> normals;
    std::vector<Vector2> texcoords;

    // Top Face
    positions.emplace_back(Vector3(-1.0f, 1.0f, -1.0f) * scale);
    positions.emplace_back(Vector3(-1.0f, 1.0f, 1.0f) * scale);
    positions.emplace_back(Vector3(1.0f, 1.0f, 1.0f) * scale);
    positions.emplace_back(Vector3(1.0f, 1.0f, -1.0f) * scale);
    colors.emplace_back(Vector3(1.0f, 0.0f, 0.0f));
    colors.emplace_back(Vector3(1.0f, 0.0f, 0.0f));
    colors.emplace_back(Vector3(1.0f, 0.0f, 0.0f));
    colors.emplace_back(Vector3(1.0f, 0.0f, 0.0f));
    normals.emplace_back(Vector3(0.0f, 1.0f, 0.0f));
    normals.emplace_back(Vector3(0.0f, 1.0f, 0.0f));
    normals.emplace_back(Vector3(0.0f, 1.0f, 0.0f));
    normals.emplace_back(Vector3(0.0f, 1.0f, 0.0f));
    texcoords.emplace_back(Vector2(0.0f, 0.0f));
    texcoords.emplace_back(Vector2(1.0f, 0.0f));
    texcoords.emplace_back(Vector2(1.0f, 1.0f));
    texcoords.emplace_back(Vector2(0.0f, 1.0f));

    // Bottom Face
    positions.emplace_back(Vector3(-1.0f, -1.0f, -1.0f) * scale);
    positions.emplace_back(Vector3(1.0f, -1.0f, -1.0f) * scale);
    positions.emplace_back(Vector3(1.0f, -1.0f, 1.0f) * scale);
    positions.emplace_back(Vector3(-1.0f, -1.0f, 1.0f) * scale);
    colors.emplace_back(Vector3(0.0f, 1.0f, 0.0f));
    colors.emplace_back(Vector3(0.0f, 1.0f, 0.0f));
    colors.emplace_back(Vector3(0.0f, 1.0f, 0.0f));
    colors.emplace_back(Vector3(0.0f, 1.0f, 0.0f));
    normals.emplace_back(Vector3(0.0f, -1.0f, 0.0f));
    normals.emplace_back(Vector3(0.0f, -1.0f, 0.0f));
    normals.emplace_back(Vector3(0.0f, -1.0f, 0.0f));
    normals.emplace_back(Vector3(0.0f, -1.0f, 0.0f));
    texcoords.emplace_back(Vector2(0.0f, 0.0f));
    texcoords.emplace_back(Vector2(1.0f, 0.0f));
    texcoords.emplace_back(Vector2(1.0f, 1.0f));
    texcoords.emplace_back(Vector2(0.0f, 1.0f));

    // Front Face
    positions.emplace_back(Vector3(-1.0f, -1.0f, -1.0f) * scale);
    positions.emplace_back(Vector3(-1.0f, 1.0f, -1.0f) * scale);
    positions.emplace_back(Vector3(1.0f, 1.0f, -1.0f) * scale);
    positions.emplace_back(Vector3(1.0f, -1.0f, -1.0f) * scale);
    colors.emplace_back(Vector3(0.0f, 0.0f, 1.0f));
    colors.emplace_back(Vector3(0.0f, 0.0f, 1.0f));
    colors.emplace_back(Vector3(0.0f, 0.0f, 1.0f));
    colors.emplace_back(Vector3(0.0f, 0.0f, 1.0f));
    normals.emplace_back(Vector3(0.0f, 0.0f, -1.0f));
    normals.emplace_back(Vector3(0.0f, 0.0f, -1.0f));
    normals.emplace_back(Vector3(0.0f, 0.0f, -1.0f));
    normals.emplace_back(Vector3(0.0f, 0.0f, -1.0f));
    texcoords.emplace_back(Vector2(0.0f, 0.0f));
    texcoords.emplace_back(Vector2(1.0f, 0.0f));
    texcoords.emplace_back(Vector2(1.0f, 1.0f));
    texcoords.emplace_back(Vector2(0.0f, 1.0f));

    // Back Face
    positions.emplace_back(Vector3(-1.0f, -1.0f, 1.0f) * scale);
    positions.emplace_back(Vector3(1.0f, -1.0f, 1.0f) * scale);
    positions.emplace_back(Vector3(1.0f, 1.0f, 1.0f) * scale);
    positions.emplace_back(Vector3(-1.0f, 1.0f, 1.0f) * scale);
    colors.emplace_back(Vector3(0.0f, 1.0f, 1.0f));
    colors.emplace_back(Vector3(0.0f, 1.0f, 1.0f));
    colors.emplace_back(Vector3(0.0f, 1.0f, 1.0f));
    colors.emplace_back(Vector3(0.0f, 1.0f, 1.0f));
    normals.emplace_back(Vector3(0.0f, 0.0f, 1.0f));
    normals.emplace_back(Vector3(0.0f, 0.0f, 1.0f));
    normals.emplace_back(Vector3(0.0f, 0.0f, 1.0f));
    normals.emplace_back(Vector3(0.0f, 0.0f, 1.0f));
    texcoords.emplace_back(Vector2(0.0f, 0.0f));
    texcoords.emplace_back(Vector2(1.0f, 0.0f));
    texcoords.emplace_back(Vector2(1.0f, 1.0f));
    texcoords.emplace_back(Vector2(0.0f, 1.0f));

    // Left Face
    positions.emplace_back(Vector3(-1.0f, -1.0f, 1.0f) * scale);
    positions.emplace_back(Vector3(-1.0f, 1.0f, 1.0f) * scale);
    positions.emplace_back(Vector3(-1.0f, 1.0f, -1.0f) * scale);
    positions.emplace_back(Vector3(-1.0f, -1.0f, -1.0f) * scale);
    colors.emplace_back(Vector3(1.0f, 1.0f, 0.0f));
    colors.emplace_back(Vector3(1.0f, 1.0f, 0.0f));
    colors.emplace_back(Vector3(1.0f, 1.0f, 0.0f));
    colors.emplace_back(Vector3(1.0f, 1.0f, 0.0f));
    normals.emplace_back(Vector3(-1.0f, 0.0f, 0.0f));
    normals.emplace_back(Vector3(-1.0f, 0.0f, 0.0f));
    normals.emplace_back(Vector3(-1.0f, 0.0f, 0.0f));
    normals.emplace_back(Vector3(-1.0f, 0.0f, 0.0f));
    texcoords.emplace_back(Vector2(0.0f, 0.0f));
    texcoords.emplace_back(Vector2(1.0f, 0.0f));
    texcoords.emplace_back(Vector2(1.0f, 1.0f));
    texcoords.emplace_back(Vector2(0.0f, 1.0f));

    // Right Face
    positions.emplace_back(Vector3(1.0f, -1.0f, 1.0f) * scale);
    positions.emplace_back(Vector3(1.0f, -1.0f, -1.0f) * scale);
    positions.emplace_back(Vector3(1.0f, 1.0f, -1.0f) * scale);
    positions.emplace_back(Vector3(1.0f, 1.0f, 1.0f) * scale);
    colors.emplace_back(Vector3(1.0f, 0.0f, 1.0f));
    colors.emplace_back(Vector3(1.0f, 0.0f, 1.0f));
    colors.emplace_back(Vector3(1.0f, 0.0f, 1.0f));
    colors.emplace_back(Vector3(1.0f, 0.0f, 1.0f));
    normals.emplace_back(Vector3(1.0f, 0.0f, 0.0f));
    normals.emplace_back(Vector3(1.0f, 0.0f, 0.0f));
    normals.emplace_back(Vector3(1.0f, 0.0f, 0.0f));
    normals.emplace_back(Vector3(1.0f, 0.0f, 0.0f));
    texcoords.emplace_back(Vector2(0.0f, 0.0f));
    texcoords.emplace_back(Vector2(1.0f, 0.0f));
    texcoords.emplace_back(Vector2(1.0f, 1.0f));
    texcoords.emplace_back(Vector2(0.0f, 1.0f));

    MeshData meshData;
    for (size_t i = 0; i < positions.size(); i++) {
        Vertex v;
        v.position = positions[i];
        v.normalModel = normals[i];
        v.texcoord = texcoords[i];
        meshData.vertices.emplace_back(v);
    }

    meshData.indices = {
        0,  1,  2,  0,  2,  3, 
        4,  5,  6,  4,  6,  7, 
        8,  9,  10, 8,  10, 11,
        12, 13, 14, 12, 14, 15,
        16, 17, 18, 16, 18, 19,
        20, 21, 22, 20, 22, 23 
    };

    return meshData;
}

MeshData GeometryGenerator::MakeCylinder(const float bottomRadius, const float topRadius,
                                         float height,
                                     int numSlices) {
    const float dTheta = -DirectX::XM_2PI / float(numSlices);
    
    MeshData meshData;
    std::vector<Vertex> &vertices = meshData.vertices;
    for (int i = 0; i <= numSlices; i++) {
        Vertex v;
        v.position = Vector3::Transform(Vector3(bottomRadius, -0.5f * height, 0.0f),
                                        Matrix::CreateRotationY(dTheta * float(i)));

        v.normalModel = v.position - Vector3(0.0f, -0.5f * height, 0.0f);
        v.normalModel.Normalize();
        v.texcoord = Vector2(float(i) / numSlices, 1.0f);

        vertices.emplace_back(v);
    }

    for (int i = 0; i <= numSlices; i++) {
        Vertex v;
        v.position = Vector3::Transform(Vector3(topRadius, 0.5f * height, 0.0f),
                                        Matrix::CreateRotationY(dTheta * float(i)));
        v.normalModel = v.position - Vector3(0.0f, 0.5f * height, 0.0f);
        v.normalModel.Normalize();
        v.texcoord = Vector2(float(i) / numSlices, 0.0f);

        vertices.emplace_back(v);
    }

    std::vector<uint32_t> &indices = meshData.indices;

    for (int i = 0; i < numSlices; i++) {
        indices.emplace_back(i);
        indices.emplace_back(i + numSlices + 1);
        indices.emplace_back(i + 1 + numSlices + 1);

        indices.emplace_back(i);
        indices.emplace_back(i + 1 + numSlices + 1);
        indices.emplace_back(i + 1);
    }

    return meshData;
}

MeshData GeometryGenerator::MakeSphere(const float radius, const int numSlices, const int numStacks,
                                       const Vector2 texScale) {
    // http://www.songho.ca/opengl/gl_sphere.html
    // https://www.opengl-tutorial.org/intermediate-tutorials/tutorial-13-normal-mapping/
    const float dTheta = -DirectX::XM_2PI / float(numSlices);
    const float dPhi = -DirectX::XM_PI / float(numStacks);

    MeshData meshData;

    std::vector<Vertex> &vertices = meshData.vertices;
    for (int i = 0; i <= numStacks; i++) {
        // X_Y Plane
        Vector3 stackStartPoint =
            Vector3::Transform(Vector3(0.0f, -radius, 0.0f), Matrix::CreateRotationZ(dPhi * i));

        for (int j = 0; j <= numSlices; j++) {
            // X_Z Plane
            Vertex v;
            v.position =
                Vector3::Transform(stackStartPoint, Matrix::CreateRotationY(dTheta * float(i)));
            v.normalModel = v.position;
            v.normalModel.Normalize();
            v.texcoord = Vector2(float(j) / numSlices, 1.0f - float(i) / numStacks) * texScale;

            Vector3 biTangent = Vector3(0.0f, 1.0f, 0.0f);
            Vector3 normalOrth = v.normalModel - biTangent.Dot(v.normalModel) * v.normalModel;
            normalOrth.Normalize();
            v.tangentModel = biTangent.Cross(normalOrth);
            v.tangentModel.Normalize();
            vertices.emplace_back(v);
        }
    }
    
    return meshData;
}

MeshData GeometryGenerator::MakeTetrahedron() {
    const float X = 0.525731f;
    const float Z = 0.850651f;

    MeshData meshData;
    
    std::vector<Vector3> position = { 
        Vector3(-X, 0.0f, Z), Vector3(X, 0.0f, Z),   Vector3(-X, 0.0f, -Z),
        Vector3(X, 0.0f, -Z), Vector3(0.0f, Z, X),   Vector3(0.0f, Z, -X),
        Vector3(0.0f, -Z, X), Vector3(0.0f, -Z, -X), Vector3(Z, X, 0.0f),
        Vector3(-Z, X, 0.0f), Vector3(Z, -X, 0.0f),  Vector3(-Z, -X, 0.0f)
    };

    for (size_t i = 0; i < position.size(); i++) {
        Vertex v;
        v.position = position[i];
        v.normalModel = v.position;
        v.normalModel.Normalize();
        meshData.vertices.emplace_back(v);
    }

    meshData.indices = {
        1, 4,  0, 4, 9, 0,  4, 5, 9,  8, 5, 4,  1,  8,  4, 
        1, 10, 8,  10, 3, 8, 8,  3, 5, 3, 2,  5, 3, 7,  2, 
        3, 10, 7,  10, 6, 7, 6,  11, 7,  6, 0, 11, 6, 1, 0, 
        10, 1, 6, 11, 0, 9, 2,  11, 9,  5, 2, 9,  11, 2,  7
    };

    return meshData;
}

MeshData GeometryGenerator::MakeIcosahedron() { 
    const float a = 1.0f;
    const float x = sqrt(3.0f) / 3.0f * a;
    const float d = sqrt(3.0f) / 6.0f * a; // = x / 2
    const float h = sqrt(6.0f) / 3.0f * a;

    std::vector<Vector3> points = {
        {0.0f, x, 0.0f}, 
        {-0.5f * a, -d, 0.0f}, 
        {+0.5f * a, -d, 0.0f}, 
        {0.0f, 0.0f, h}
    };

    Vector3 center = Vector3(0.0f);

    for (int i = 0; i < 4; i++) {
        center += points[i];
    }
    center /= 4.0f;

    for (int i = 0; i < 4; i++) {
        points[i] -= center;
    }

    MeshData meshData;

    for (int i = 0; i < points.size(); i++) {

        Vertex v;
        v.position = points[i];
        v.normalModel = v.position;
        v.normalModel.Normalize();

        meshData.vertices.push_back(v);
    }

    meshData.indices = {0, 1, 2, 3, 2, 1, 0, 3, 1, 0, 2, 3};
    return meshData;
}


} // namespace Luna
