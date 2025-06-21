#pragma once
#include <directxtk/SimpleMath.h>

#define MAX_LIGHTS 3

namespace Luna {

using DirectX::SimpleMath::Matrix;
using DirectX::SimpleMath::Vector2;
using DirectX::SimpleMath::Vector3;

// Vertex & Geometry
__declspec(align(256)) struct MeshConstants {
    Matrix world;       // 16
    Matrix worldIT;     // 16
    float heightScale;  // 4
    Vector3 dummy;        // 8
};

static_assert((sizeof(MeshConstants) % 16) == 0,
              "Constant Buffer size must be 16-byte aligned");

// This is more like debugging purpose
__declspec(align(256)) struct MaterialConstants {
    float expose = 1.0f;    // 4
    float gamma = 2.2f;     // 4
    int reverseNormalY = 0; // 4
    int isParallax;            // 4
};

static_assert((sizeof(MaterialConstants) % 16) == 0,
              "Constant Buffer size must be 16-byte aligned");

struct Light {
    Vector3 radiance = Vector3(0.0f);              // 12
    float fallOffStart = 0.0f;                     // 4
    Vector3 direction = Vector3(0.0f, 0.0f, 1.0f); // 12
    float fallOffEnd = 10.0f;                      // 4
    Vector3 position = Vector3(0.0f, 0.0f, -2.0f); // 12
    float spotPower = 100.0f;                  // 4
};

static_assert((sizeof(Light) % 16) == 0,
              "Constant Buffer size must be 16-byte aligned");

__declspec(align(256)) struct GlobalConstants {
    Matrix view;                // 16
    Matrix proj;                // 16
    Matrix viewProj;            // 16
    Vector3 eyeWorld;           // 12
    float strengthIBL = 1.0f;   //  4
    int textureToDraw = 0;      //  4 - 0: Env, 1: Specular, 2: Irradiance  
    float envLodBias = 0.0f;    // 4
    float dummy1;                // 4
    float dummy2;
    Light lights[MAX_LIGHTS];   // 48 * 3
};

static_assert((sizeof(GlobalConstants) % 16) == 0,
              "Constant Buffer size must be 16-byte aligned");
} // namespace Luna


__declspec(align(256)) struct ImageFilterConstData {
    float dx;
    float dy;
    float threshold;
    float strength;
    float option1;
    float option2;
    float option3;
    float option4;
};

static_assert((sizeof(ImageFilterConstData) % 16) == 0,
              "Constant Buffer size must be 16-byte aligned");