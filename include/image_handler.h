#pragma once
#include <cstdint>
#include <string>
#include <vector>

struct RGBA
{
    uint8_t r, g, b, a;
};

using ColorIndex = uint8_t; // tile index or color index. If you have more than 255, don't.
const size_t MAX_COLORS = 1 << (sizeof(ColorIndex) * 8);

using Graphics = std::vector<std::vector<ColorIndex>>;
using Image = std::vector<std::vector<RGBA>>;
using Palette = std::vector<RGBA>;


class ImageHandler
{
public:
    static const int channel_num = 4;

    static bool ImageHandler::load_image(const std::string& filename, std::vector<unsigned char>& out_image, int& width, int& height);
    static bool write_image(const std::string& filename, const std::vector<unsigned char>& image, const int width, const int height);
    static bool write_matrix(const std::string& filename, const std::vector<unsigned char>& image, int width, int height);
private:
    static const std::string sample_location;
    static const std::string result_location;
};
