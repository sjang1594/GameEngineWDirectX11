#pragma once
#include <vector>
#include <string>
#include <dxgiformat.h>

namespace Luna {
using namespace std;
using namespace DirectX;
class ImageUtils {
  public:
      static ImageUtils& GetInstance() { 
          static ImageUtils instance;
          return instance;
      }

      static void ReadImage(const std::string fileName, std::vector<uint8_t> &image, 
                            int& width, int& height);

      static void ReadEXRImage(const std::string fileName, std::vector<uint8_t> &image, 
                               int &width, int &height, DXGI_FORMAT &pixelFormat);
 private:
    ImageUtils() = default;
    ~ImageUtils() = default;
  // disable copy / move
    ImageUtils(ImageUtils const &) = delete;
    ImageUtils(ImageUtils &&) = delete;
    ImageUtils &operator=(ImageUtils const &) = delete;
    ImageUtils &operator=(ImageUtils &&) = delete;
};
} // namespace Luna