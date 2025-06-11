#include "pch.h"
#include "ImageUtils.h"
#include "D3D11Utils.h"
#include <DirectXTex.h>
#include <DirectXTexEXR.h>
#include <fp16.h>

#define STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image.h"
#include "stb_image_write.h"

namespace Luna {
void ImageUtils::ReadImage(const std::string fileName, std::vector<uint8_t> &image,
                           int &width, int &height) {
    int channels;

    unsigned char *img = stbi_load(fileName.c_str(), &width, &height, &channels, 0);

    // assert(channels == 4);

    cout << fileName << " " << width << " " << height << " " << channels << endl;

    // 4채널로 만들어서 복사
    image.resize(width * height * 4);

    if (channels == 1) {
        for (size_t i = 0; i < width * height; i++) {
            uint8_t g = img[i * channels + 0];
            for (size_t c = 0; c < 4; c++) {
                image[4 * i + c] = g;
            }
        }
    } else if (channels == 3) {
        for (size_t i = 0; i < width * height; i++) {
            for (size_t c = 0; c < 3; c++) {
                image[4 * i + c] = img[i * channels + c];
            }
            image[4 * i + 3] = 255;
        }
    } else if (channels == 4) {
        for (size_t i = 0; i < width * height; i++) {
            for (size_t c = 0; c < 4; c++) {
                image[4 * i + c] = img[i * channels + c];
            }
        }
    } else {
        std::cout << "Failed To Read " << channels << " channels" << endl;
    }
}

void ImageUtils::ReadEXRImage(const std::string fileName, std::vector<uint8_t> &image, int &width,
                              int &height, DXGI_FORMAT &pixelFormat) {

    const std::wstring wFileName(fileName.begin(), fileName.end());
    TexMetadata metaData;
    ThrowIfFailed(GetMetadataFromEXRFile(wFileName.c_str(), metaData));
    
    ScratchImage scratchImg;
    ThrowIfFailed(LoadFromEXRFile(wFileName.c_str(), nullptr, scratchImg));
    width = static_cast<int>(metaData.width);
    height = static_cast<int>(metaData.height);
    pixelFormat = metaData.format;

    cout << fileName << " " << metaData.width << " " << metaData.height << metaData.format << endl;

    image.resize(scratchImg.GetPixelsSize());
    memcpy(image.data(), scratchImg.GetPixels(), image.size());

    vector<float> f32(image.size() / 2);
    uint16_t *f16 = (uint16_t *)image.data();
    for (int i = 0; i < image.size() / 2; i++) {
        f32[i] = fp16_ieee_to_fp32_value(f16[i]);
    }

    const float minValue = *std::min_element(f32.begin(), f32.end());
    const float maxValue = *std::max_element(f32.begin(), f32.end());

    cout << minValue << " " << maxValue << endl;
}
} // namespace Lunas