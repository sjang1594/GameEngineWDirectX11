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
    int useHeightMap = 0;
    float heightScale = 0.0f;
    Vector2 dummy;
};

_declspec(align(256)) struct MaterialConstants {
    Vector3 albedoFactor = Vector3(1.0f);
    float roughnessFactor = 1.0f;
    float metallicFactor = 1.0f;
    Vector3 emissionFactor = Vector3(0.0f);
    int useAlbedoMap = 0;     // 4
    int useNormalMap = 0;     // 4
    int useAOMap = 0;         // 4
    int invertNormalMapY = 0; // 4
    int useMetallicMap = 0;   // 4
    int useRoughnessMap = 0;  // 4
    int useEmissiveMap = 0;   // 4
    float dummy = 0.0f;       // 8
};

struct Light {};

_declspec(align(256)) struct GlobalConstants {
    Matrix view; 
    
    Matrix projection;
    Matrix invProjection;

    Matrix viewProjection;
    Matrix inverseViewProjection;

    Vector3 eyeWorld;
    float strengthIBL = 0.0f;
    int textureToDraw = 0;   // 0: Env, 1: Specular, 2: Irradiance, 그외: 검은색
    float envLodBias = 0.0f; // 환경맵 LodBias
    float lodBias = 2.0f;    // 다른 물체들 LodBias
    float dummy2 = 0.0f;

    Light lights[MAX_LIGHT];
};

} // namespace Luna