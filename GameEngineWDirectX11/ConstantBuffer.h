#pragma once
#include <directxtk/SimpleMath.h>

#define MAX_LIGHT 3
#define LIGHT_OFF 0x00
#define LIGHT_DIRECTIONAL 0x01
#define LIGHT_POINT 0x02
#define LIGHT_SPOT 0x04

namespace Luna {
using DirectX::SimpleMath::Matrix;
using DirectX::SimpleMath::Vector2;
using DirectX::SimpleMath::Vector3;

_declspec(align(256)) struct MeshConstants {
    Matrix world;   // Model -> World
    Matrix worldIT; // Inverse of above 
};

_declspec(align(256)) struct MaterialConstants {
    Vector3 albedoFactor = Vector3(1.0f);
    float roughnessFactor = 1.0f;
    float metallicFactor = 1.0f;
    Vector3 emissionFactor = Vector3(0.0f);
};

struct Light {};

_declspec(align(256)) struct GlobalConstants {
    Matrix view; 
    
    Matrix projection;
    Matrix invProjection;

    Matrix viewProjection;
    Matrix inverseViewProjection;

    Vector3 eyeWorld;
    
    Light lights[MAX_LIGHT];
};

} // namespace Luna