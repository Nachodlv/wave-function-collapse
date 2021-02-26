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

    friend bool operator==(const TileBitMap& lhs, const TileBitMap& rhs) { return lhs.r == rhs.r && lhs.g == rhs.g && lhs.b == rhs.b && lhs.a == rhs.a; }
    friend bool operator<(const TileBitMap& lhs, const TileBitMap& rhs) { return (lhs.r + lhs.g + lhs.b + lhs.a) < (rhs.r + rhs.g + rhs.b + rhs.a); }

    char sum_pixels() const {return r + g + b + a; }

    template<typename F>
    void map(F f)
    {
        f(r);
        f(g);
        f(b);
        f(a);
    }
};

class TileBitMapGroup
{
public:
    TileBitMapGroup(const std::vector<std::shared_ptr<TileBitMap>> tiles): tiles(tiles) {}
    const std::vector<std::shared_ptr<TileBitMap>> tiles;

    //TODO make the operators friend
    bool operator==(const TileBitMapGroup& rhs) const
    {
        if(tiles.size() != rhs.tiles.size()) return false;
        for (int i = 0; i < tiles.size(); ++i)
        {
            if(!(*tiles[i].get() == *rhs.tiles[i].get()))
                return false;
        }
        return true;
    }

    bool operator<(const TileBitMapGroup& rhs) const
    {
        return sum_tiles(tiles) < sum_tiles(rhs.tiles);
    }

    char sum_tiles(const std::vector<std::shared_ptr<TileBitMap>>& tiles) const
    {
        char total = 0;
        for (const auto tile : tiles)
        {
            total += tile->sum_pixels();
        }
        return total;
    }
};

class WfcBitmap
{
public:
    void wave_function_with_bitmaps();
    void wave_function_with_group_bitmaps(int group_size);

private:
    void from_image_to_tiles(const std::vector<unsigned char>& image, int width, int height, std::vector<std::vector<std::shared_ptr<TileBitMap>>>& tiles) const;
    void from_tiles_to_image(std::vector<std::vector<const TileBitMap*>>& tiles, std::vector<unsigned char>& image);
    void from_matrix_to_tiles(std::vector<std::vector<std::set<const TileBitMap*>>>& matrix, std::vector<std::vector<const TileBitMap*>>& tiles);

};
