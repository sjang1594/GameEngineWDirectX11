#pragma once
#include <string>
#include <vector>
#include "MeshData.h"
#include "Vertex.h"

namespace Luna {
using namespace std;
using namespace DirectX;
using namespace DirectX::SimpleMath;

class GeometryGenerator {
  public:
    static vector<MeshData> ReadFromFile(std::string basePath, std::string filename);
    
    static MeshData MakeSquare(const float scale = 1.0f,
                               const Vector2 texScale = Vector2(1.0f));
    
    static MeshData MakeBox(const float scale = 1.0f);
    
    static MeshData MakeCylinder(const float bottomRadius, 
                                 const float topRadius, 
                                 float height,
                                 int numSlices);
    
    static MeshData MakeSphere(const float radius, 
                               const int numSlices, 
                               const int numStacks, 
                               const Vector2 texScale);

    static MeshData MakeTetrahedron();
    static MeshData MakeIcosahedron();
    static MeshData SubdivideToSphere(const float radius, MeshData meshData);

  private:
    static void ComputeBoundingBox(const vector<MeshData> &meshes, Vector3 &vmin,
                                   Vector3 &vmax);
    
    static void NormalizeMeshes(vector<MeshData> &meshes, const Vector3 &center,
                                float scale);
};
} // namespace Luna
