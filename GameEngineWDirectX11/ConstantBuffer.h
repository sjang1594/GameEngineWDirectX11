#pragma once
#include <directxtk/SimpleMath.h>
#include "Light.h"
#include "Material.h"

namespace Luna {

using DirectX::SimpleMath::Matrix;
using DirectX::SimpleMath::Vector3;

struct BasicVertexConstantData {
    Matrix model;
    Matrix invTranspose;
    Matrix view;
    Matrix projection;
};

static_assert((sizeof(BasicVertexConstantData) % 16) == 0,
              "Constant Buffer size must be 16-byte aligned");

struct BasicPixelConstantData {
    Vector3 eyeWorld;          // 12
    float dummy1;              // 4
    Material material;         // 64
    Light lights[MAX_LIGHTS];  // 48 * MAX_LIGHTS
    int reverseNormalMapY = 0; // 4
    float heightScale;         // 4
    float dummy2[2];           // 8
};

static_assert((sizeof(BasicPixelConstantData) % 16) == 0,
              "Constant Buffer size must be 16-byte aligned");

struct NormalVertexConstantData {
    float scale = 0.1f; // 4
    float dummy1;       // 4
    float dummy2[2];    // 8
};

static_assert((sizeof(NormalVertexConstantData) % 16) == 0,
              "Constant Buffer size must be 16-byte aligned");

struct ImageFilterConstantData {
    float dx;
    float dy;
    float threshold;
    float strength;
    float option1;
    float option2;
    float option3;
    float option4;
};

static_assert((sizeof(ImageFilterConstantData) % 16) == 0,
              "Constant Buffer size must be 16-byte aligned");

/* This is for skybox */
struct CubeMappingVertexConstantData {
    Matrix viewProj;
};

static_assert((sizeof(CubeMappingVertexConstantData) % 16) == 0,
              "Constant Buffer size must be 16-byte aligned");

struct CubeMappingPixelConstantData {
    int textureToDraw = 0; // 0: Env, 1: Specular, 2: Irridiance
    float mipLevel = 0.0f;
    float dummy1;
    float dummy2;
};

static_assert((sizeof(CubeMappingPixelConstantData) % 16) == 0,
              "Constant Buffer size must be 16-byte aligned");
} // namespace Luna
