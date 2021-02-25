#include "../include/image_handler.h"

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"

#include <iostream>

const std::string ImageHandler::sample_location = "../samples/";
const std::string ImageHandler::result_location = "../results/";

bool ImageHandler::load_image(const std::string& filename, std::vector<unsigned char>& out_image, int& width, int& height)
{
    int n;
    unsigned char* data = stbi_load((sample_location + filename).c_str(), &width, &height, &n, 4);
    if (data != nullptr)
    {
        out_image = std::vector<unsigned char>(data, data + width * height * 4);
    }
    stbi_image_free(data);
    return (data != nullptr);
}

bool ImageHandler::write_image(const std::string& filename, const std::vector<unsigned char>& image, const int width, const int height)
{
    return stbi_write_png(
        (result_location + filename).c_str(), width, height, channel_num, &image[0], width * channel_num);
}
