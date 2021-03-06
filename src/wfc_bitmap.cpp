#include "wfc_bitmap.h"

#include <cassert>
#include <iostream>
#include <memory>
#include <vector>

#include "image_handler.h"
#include "wfc.h"

void WfcBitmap::wave_function_with_bitmaps()
{
    // Step 1: Read image
    std::vector<unsigned char> image;
    int width;
    int height;
    const bool image_loaded = ImageHandler::load_image("ScaledMaze.png", image, width, height);
    if (image_loaded)
    {
        std::cout << "Width: " << width << ", Height: " << height << std::endl;
    }
    else
    {
        std::cout << "Error reading the image" << std::endl;
    }
    std::vector<std::vector<std::shared_ptr<TileBitMap>>> tiles;

    from_image_to_tiles(image, width, height, tiles);
    std::vector<std::vector<const TileBitMap*>> tile_pointers;
    Utils::from_shared_to_raw_pointer(tiles, tile_pointers);

    // Step 2: Run wfc
    std::set<CompatibleTile<const TileBitMap*>> compatible_tiles;
    WfcMap<TileBitMap, int> weights;
    std::vector<std::vector<const TileBitMap*>> result;
    const Vector output_size = {50, 50};
    Wfc<TileBitMap> wfc;
    wfc.parse_matrix(tile_pointers, compatible_tiles, weights);

    int iterations = 0;
    wfc.run(compatible_tiles, weights, output_size, result,
            [&output_size, &iterations,this](auto matrix)
            {
                ++iterations;
                if (iterations % 1000 != 0) return;
                std::vector<std::vector<const TileBitMap*>> tiles;
                std::vector<unsigned char> image;
                from_matrix_to_tiles(matrix, tiles);
                from_tiles_to_image(tiles, image);
                ImageHandler::write_image("result" + std::to_string(iterations) + ".png", image, output_size.x,
                                          output_size.y);
            });

    // Step 3: Write image
    std::vector<unsigned char> new_image;
    from_tiles_to_image(result, new_image);
    const bool image_written = ImageHandler::write_image("result.png", new_image, output_size.x, output_size.y);
    if (image_written)
    {
        std::cout << "Image written!" << std::endl;
    }
    else
    {
        std::cout << "Error writing the image" << std::endl;
    }
}

void WfcBitmap::wave_function_with_group_bitmaps(int group_size)
{
    // Step 1: Read image
    std::vector<unsigned char> image;
    int width;
    int height;
    const bool image_loaded = ImageHandler::load_image("ScaledMaze.png", image, width, height);
    if (image_loaded)
    {
        std::cout << "Width: " << width << ", Height: " << height << std::endl;
    }
    else
    {
        std::cout << "Error reading the image" << std::endl;
    }

    // Step 2: create group matrix
    std::vector<std::vector<std::shared_ptr<TileBitMapGroup>>> group_matrix;
    const int image_size = height * width * ImageHandler::channel_num;
    for (int i = 0; i < height; ++i)
    {
        std::vector<std::shared_ptr<TileBitMapGroup>> row;
        for (int j = 0; j < width; ++j)
        {
            const int position = i * width * ImageHandler::channel_num + j * ImageHandler::channel_num;
            std::vector<std::shared_ptr<TileBitMap>> group;
            for (int y = 0; y < group_size; ++y)
            {
                for (int x = 0; x < group_size; ++x)
                {
                    const int group_position = position + y * width * ImageHandler::channel_num + x *
                        ImageHandler::channel_num;
                    if (group_position >= image_size) break;
                    const unsigned char* pixel = &image[group_position];
                    group.push_back(std::make_shared<TileBitMap>(*pixel, *(pixel + 1), *(pixel + 2), *(pixel + 3)));
                }
            }
            row.push_back(std::make_shared<TileBitMapGroup>(group));
        }
        group_matrix.push_back(row);
    }

    // Step 3 run wfc
    std::vector<std::vector<const TileBitMapGroup*>> group_matrix_pointers;
    Utils::from_shared_to_raw_pointer(group_matrix, group_matrix_pointers);

    std::set<CompatibleTile<const TileBitMapGroup*>> compatible_tiles;
    WfcMap<TileBitMapGroup, int> weights;
    std::vector<std::vector<const TileBitMapGroup*>> result;
    const Vector output_size = {50, 50};
    Wfc<TileBitMapGroup> wfc;
    wfc.parse_matrix(group_matrix_pointers, compatible_tiles, weights);

    int iterations = 0;
    wfc.run(compatible_tiles, weights, output_size, result,
            [&output_size, &iterations, this](auto matrix)
            {
            });

    // Step 3: Write image
    // std::vector<unsigned char> new_image[width * height * ImageHandler::channel_num];
    //
    // for (int i = 0; i < height; ++i)
    // {
    //     for (int j = 0; j < width; ++j)
    //     {
    //         const int position = i * width * ImageHandler::channel_num + j * ImageHandler::channel_num;
    //         for (int y = 0; y < group_size; ++y)
    //         {
    //             for (int x = 0; x < group_size; ++x)
    //             {
    //                 const int group_position = position + y * width * ImageHandler::channel_num + x *
    //                     ImageHandler::channel_num;
    //                 if (group_position >= image_size) break;
    //                 const unsigned char* pixel = &image[group_position];
    //                 group.push_back(std::make_shared<TileBitMap>(*pixel, *(pixel + 1), *(pixel + 2), *(pixel + 3)));
    //             }
    //         }
    //         row.push_back(std::make_shared<TileBitMapGroup>(group));
    //     }
    //     group_matrix.push_back(row);
    // }

    // from_tiles_to_image(result, new_image);
    // const bool image_written = ImageHandler::write_image("result.png", new_image, output_size.x, output_size.y);
    // if (image_written)
    // {
    //     std::cout << "Image written!" << std::endl;
    // }
    // else
    // {
    //     std::cout << "Error writing the image" << std::endl;
    // }
}

void WfcBitmap::from_image_to_tiles(const std::vector<unsigned char>& image, int width, int height,
                                    std::vector<std::vector<std::shared_ptr<TileBitMap>>>& tiles) const
{
    for (int i = 0; i < height; ++i)
    {
        std::vector<std::shared_ptr<TileBitMap>> row;
        for (int j = 0; j < width; ++j)
        {
            const unsigned char* pixel = &image[i * width * ImageHandler::channel_num + j * ImageHandler::channel_num];
            row.push_back(std::make_shared<TileBitMap>(*pixel, *(pixel + 1), *(pixel + 2), *(pixel + 3)));
        }
        tiles.push_back(row);
    }
}

void WfcBitmap::from_tiles_to_image(std::vector<std::vector<const TileBitMap*>>& tiles,
                                    std::vector<unsigned char>& image)
{
    for (auto row : tiles)
    {
        for (const auto tile : row)
        {
            assert(tile);
            image.push_back(tile->r);
            image.push_back(tile->g);
            image.push_back(tile->b);
            image.push_back(tile->a);
        }
    }
}

void WfcBitmap::from_matrix_to_tiles(std::vector<std::vector<std::set<const TileBitMap*>>>& matrix,
                                     std::vector<std::vector<const TileBitMap*>>& tiles)
{
    for (auto row : matrix)
    {
        std::vector<const TileBitMap*> row_tiled;
        for (auto options : row)
        {
            unsigned char r = 0, g = 0, b = 0, a = 0;
            //FIXME needs to cap at 255 or it will have strange side effects
            for (const TileBitMap* tile : options)
            {
                r = r + tile->r > 255 ? 255 : r + tile->r;
                g = g + tile->g > 255 ? 255 : g + tile->g;
                b = b + tile->b > 255 ? 255 : b + tile->b;
                a = a + tile->a > 255 ? 255 : a + tile->a;
            }
            std::shared_ptr<TileBitMap> tile = std::make_shared<TileBitMap>(r, g, b, a);
            tile->map([options](unsigned char& color) { color /= options.size(); });
            row_tiled.push_back(tile.get());
        }
        tiles.push_back(row_tiled);
    }
}
