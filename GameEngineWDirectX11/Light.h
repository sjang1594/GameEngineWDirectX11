#pragma once
#include <directxtk/SimpleMath.h>

#define MAX_LIGHTS 3

namespace Luna {
using DirectX::SimpleMath::Vector3;
class Light {
  // TODO: Create BoundingBox Sphere such that it can change position/rotation
  public:
    Vector3 strength = Vector3(0.0f);
    float fallOffStart = 0.0f;
    Vector3 direction = Vector3(0.0f, 0.0f, 1.0f);
    float fallOffEnd = 10.0f;
    Vector3 position = Vector3(0.0f, 0.0f, -2.0f);
    float spotPower = 100.0f;
};
} // namespace Luna
