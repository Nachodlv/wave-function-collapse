#include <iostream>
#include <memory>


#include "image_handler.h"
#include "printer.h"
#include "wfc.h"

class Tile
{
public:
    Tile(char character, const ConsoleColors color): character(character), color(color)
    {
    }

    ConsoleColors get_color() const { return color; }
    char get_character() const { return character; }


    bool operator==(const Tile& rhs) const {return character == rhs.get_character();}
    bool operator<(const Tile& rhs) const {return character < rhs.get_character();}

private:
    const char character;
    const ConsoleColors color;
};

class TileBitMap
{
public :
    TileBitMap(unsigned char r, unsigned char g, unsigned char b, unsigned char a):
        r(r), g(g), b(b), a(a)
    {
    }
    unsigned char r;
    unsigned char g;
    unsigned char b;
    unsigned char a;

    bool operator==(const TileBitMap& rhs) const { return r == rhs.r && g == rhs.g && b == rhs.b && a == rhs.a; }
    bool operator<(const TileBitMap& rhs) const { return (r + g + b + a) < (rhs.r + rhs.g + rhs.b + rhs.a); }

    template<typename F>
    void map(F f)
    {
        f(r);
        f(g);
        f(b);
        f(a);
    }
};

void wave_function_with_chars()
{
    const Printer printer;

    // Initialize input matrix
    const Tile* l = new Tile('L', green);
    const Tile* c = new Tile('C', yellow);
    const Tile* s = new Tile('S', blue);
    const Tile* m = new Tile('M', gray);
    const Tile* r = new Tile('R', blue_green);
    const Tile* h = new Tile('H', red);
    const Tile* b = new Tile('B', black);

    const std::vector<std::vector<const Tile*>> complex_input = {
        {l, l, l, l},
        {l, l, l, l},
        {l, l, l, l},
        {l, c, c, l},
        {c, s, s, c},
        {s, s, s, s},
        {s, s, s, s},
    };
    const std::vector<std::vector<const Tile*>> full_input = {
        {l, m, m, m, m, m, m, l},
        {l, r, l, l, l, l, l, l},
        {l, r, l, l, l, h, h, l},
        {h, r, c, h, h, c, c, l},
        {c, s, s, c, c, s, s, c},
        {s, s, s, s, s, b, s, s},
        {s, s, s, s, s, s, s, s},
    };

    const std::vector<std::vector<const Tile*>> easy_input = {
        {l},
        {c},
        {s}
    };

    std::set<CompatibleTile<const Tile*>> compatible_tiles;
    WfcMap<Tile, int> weights;
    std::vector<std::vector<const Tile*>> result;
    Wfc<Tile> wfc;
    wfc.parse_matrix(full_input, compatible_tiles, weights);

    wfc.run(compatible_tiles, weights, {25, 25}, result,
            [&printer](auto matrix)
            {
                // printer.print_non_collapsed_matrix<const Tile*>(matrix);
                // std::cout << "\n-----------------------\n" << std::endl;
            });

    // Print matrix
    std::cout << std::endl;
    printer.print_matrix<const Tile*>(result, "  ");
}

// FIXME this is not correct, I think
void from_image_to_tiles(const std::vector<unsigned char>& image, int width, int height, std::vector<std::vector<std::shared_ptr<TileBitMap>>>& tiles)
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

void from_tiles_to_image(std::vector<std::vector<const TileBitMap*>>& tiles, std::vector<unsigned char>& image)
{
    for (auto row : tiles)
    {
        for(const auto tile : row)
        {
            assert(tile);
            image.push_back(tile->r);
            image.push_back(tile->g);
            image.push_back(tile->b);
            image.push_back(tile->a);
        }
    }
}

void from_matrix_to_tiles(std::vector<std::vector<std::set<const TileBitMap*>>>& matrix, std::vector<std::vector<const TileBitMap*>>& tiles)
{
    for(auto row : matrix)
    {
        std::vector<const TileBitMap*> row_tiled;
        for (auto options : row)
        {
            unsigned char r = 0, g = 0, b = 0, a = 0;
            //FIXME needs to cap at 255 or it will have strange side effects
            for(const TileBitMap* tile : options)
            {
                r = r + tile->r > 255 ? 255 : r + tile->r;
                g = g + tile->g > 255 ? 255 : g + tile->g;
                b = b + tile->b > 255 ? 255 : b + tile->b;
                a = a + tile->a > 255 ? 255 : a + tile->a;
            }
            std::shared_ptr<TileBitMap> tile = std::make_shared<TileBitMap>(r, g, b, a);
            tile->map([options](unsigned char & color){ color /= options.size();});
            row_tiled.push_back(tile.get());
        }
        tiles.push_back(row_tiled);
    }
}

void wave_function_with_bitmaps()
{
    std::vector<unsigned char> image;
    int width;
    int height;
    const bool image_loaded = ImageHandler::load_image("Village.png", image, width, height);
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
    for(auto row : tiles)
    {
        std::vector<const TileBitMap*> row_pointers;
        for(auto tile : row)
        {
            row_pointers.push_back(tile.get());
        }
        tile_pointers.push_back(row_pointers);
    }


    std::set<CompatibleTile<const TileBitMap*>> compatible_tiles;
    WfcMap<TileBitMap, int> weights;
    std::vector<std::vector<const TileBitMap*>> result;
    const Vector output_size = {50, 50};
    Wfc<TileBitMap> wfc;
    wfc.parse_matrix(tile_pointers, compatible_tiles, weights);

    int iterations = 0;
    wfc.run(compatible_tiles, weights, output_size, result,
            [&output_size, &iterations](auto matrix)
            {
                ++iterations;
                if(iterations % 100 != 0) return;
                std::vector<std::vector<const TileBitMap*>> tiles;
                std::vector<unsigned char> image;
                from_matrix_to_tiles(matrix, tiles);
                from_tiles_to_image(tiles, image);
                const bool image_written = ImageHandler::write_image("result" + std::to_string(iterations) + ".png", image, output_size.x, output_size.y);
            });

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

int main()
{
    wave_function_with_bitmaps();
    return 0;
}
