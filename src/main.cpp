#include <iostream>

#include "printer.h"
#include "wfc.h"

class Tile
{
public:
    Tile(const char character, const ConsoleColors color): character(character), color(color) {}

    ConsoleColors get_color() const {return color;}
    char get_character() const {return character;}

    //FIXME try to remove me
    bool operator<(const Tile& rhs) const { return character < rhs.character;}

private:
    const char character;
    const ConsoleColors color;
};

int main()
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

    std::set<CompatibleTile<Tile>> compatible_tiles;
    std::map<const Tile*, int> weights;
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

    return 0;
}