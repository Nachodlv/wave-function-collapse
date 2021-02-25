#include "../include/image_handler.h"

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"

#include <iostream>

namespace
{
	constexpr const char* const sample_location = "../samples/";
	constexpr const char* const result_location = "../results/";
}

bool ImageHandler::load_image(const std::string& filename, std::vector<unsigned char>& out_image, int& width, int& height)
{
    int n;
    unsigned char* data = stbi_load((std::string(sample_location) + filename).c_str(), &width, &height, &n, 4);
    if (data != nullptr)
    {
        out_image = std::vector<unsigned char>(data, data + width * height * 4);
    }
    stbi_image_free(data);
    return (data != nullptr);
}

bool ImageHandler::write_image(const std::string& filename, const std::vector<unsigned char>& image, int width, int height)
{
    return stbi_write_png(
        (std::string(result_location) + filename).c_str(), width, height, channel_num, &image[0], width * channel_num);
}
