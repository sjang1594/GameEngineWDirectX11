#pragma once
#include <directxtk/SimpleMath.h>

namespace Luna {

using DirectX::SimpleMath::Matrix;
using DirectX::SimpleMath::Vector3;

struct Material {
    Vector3 ambient = Vector3(0.0f);                 // 12
    float shininess = 0.01f;                         // 4
    Vector3 diffuse = Vector3(0.0f);                 // 12
    float dummy1;                                    // 4
    Vector3 specular = Vector3(1.0f);                // 12
    float dummy2;                                    // 4
    Vector3 fresnelR0 = Vector3(1.0f, 0.71f, 0.29f); // Gold
    float dummy3;
};

} // namespace Luna