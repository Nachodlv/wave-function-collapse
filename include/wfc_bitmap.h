#pragma once
#include <memory>
#include <set>
#include <vector>

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

    friend bool operator==(const TileBitMap& rhs) const { return r == rhs.r && g == rhs.g && b == rhs.b && a == rhs.a; }
    friend bool operator<(const TileBitMap& rhs) const { return (r + g + b + a) < (rhs.r + rhs.g + rhs.b + rhs.a); }

    template<typename F>
    void map(F f)
    {
        f(r);
        f(g);
        f(b);
        f(a);
    }
};

class WfcBitmap
{
public:
    void wave_function_with_bitmaps();

private:
    void from_image_to_tiles(const std::vector<unsigned char>& image, int width, int height, std::vector<std::vector<std::shared_ptr<TileBitMap>>>& tiles) const;
    void from_tiles_to_image(std::vector<std::vector<const TileBitMap*>>& tiles, std::vector<unsigned char>& image);
    void from_matrix_to_tiles(std::vector<std::vector<std::set<const TileBitMap*>>>& matrix, std::vector<std::vector<const TileBitMap*>>& tiles);

};
