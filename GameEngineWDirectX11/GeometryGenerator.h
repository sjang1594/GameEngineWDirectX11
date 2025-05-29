#pragma once
#include <directxtk/SimpleMath.h>
#include <string>
#include <vector>

#include "MeshData.h"

namespace Luna {
using DirectX::SimpleMath::Vector2;
using DirectX::SimpleMath::Matrix;

// Comments: this can be done in GPU Side as well.
// https://www.shadertoy.com/view/Xds3zN

class GeometryGenerator {
  public:
    static std::vector<MeshData> ReadFromFile(std::string basePath, std::string fileName,
                                         bool revertNormals = false);

    static MeshData MakeSquare(const float scale = 1.0f, const Vector2 texScale = Vector2(1.0f));
    static MeshData MakeSquareGrid(const int numSlices, const int numStacks,
                                   const float scale = 1.0f,
                                   const Vector2 texScale = Vector2(1.0f));
    static MeshData MakeCube(const float scale = 1.0f);
    static MeshData MakeCylinder(const float bottomRadius, const float topRadius, float height,
                                 int numSlices);
    static MeshData MakeSphere(const float radius, const int numSlices, const int numStacks,
                               const Vector2 texScale = Vector2(1.0f));

    static MeshData MakeTetrahedron();

    static MeshData MakeIcosahedron();
};
} // namespace Luna
