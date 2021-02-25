#pragma once
#include <map>
#include <set>
#include <stack>
#include <tuple>
#include <vector>
#include <windows.h>

#include "Utils.h"

// I might need to be this struct to its own file
struct Vector
{
	
	constexpr Vector() : x(0), y(0) {}
	constexpr Vector(int inX, int inY) : x(inX), y(inY) {}
	
    int x;
    int y;

    friend bool operator<(const Vector& lhs, const Vector& rhs)
    {
        return std::tie(lhs.x, lhs.y) < std::tie(rhs.x, rhs.y);
    }

    friend bool operator==(const Vector& lhs, const Vector& rhs)
    {
        return lhs.x == rhs.x && lhs.y == rhs.y;
    }
};

template <typename T>
struct CompatibleTile
{
    T tile1;
    T tile2;
    Vector direction;

    bool operator<(const CompatibleTile& rhs) const
    {
        return std::tie(tile1, tile2, direction) < std::tie(rhs.tile1, rhs.tile2, rhs.direction);
    }
};

// Global directions
// FIXME Should be inside an anonymous namespace? What are the pros and cons?
namespace Directions
{
    constexpr Vector up = {0, 1};
    constexpr Vector left = {-1, 0};
    constexpr Vector down = {0, -1};
    constexpr Vector right = {1, 0};
}

template<class T> struct ptr_less
{
    bool operator()(const T* lhs, const T* rhs) const { return *lhs < *rhs; }
};

template <class T, class G>
using WfcMap = std::map<const T*, G, ptr_less<T>>;

// FIXME create a constructor for the class
template <class T>
class Wfc
{
public:
    /**
    * Parses the matrix.
    * Calculates the weight of each tiles. How many times each cell is repeated.
    * Checks which pairs of tiles can be placed next to each other and in which direction.
    *
    * @param matrix the matrix that will be parsed
    * @param out_compatible_tiles the pairs of tiles calculated
    * @param out_weights how many times the tile is repeated
    */
    void parse_matrix(const std::vector<std::vector<const T*>>& matrix,
                      OUT std::set<CompatibleTile<const T*>>& out_compatible_tiles,
                      OUT WfcMap<T, int>& out_weights) const
    {
        const int width = matrix.size();
        for (int i = 0; i < width; ++i)
        {
            const int height = matrix[i].size();
            for (int j = 0; j < height; ++j)
            {
                const T* tile = matrix[i][j];

                // sum the weight
                auto iterator = out_weights.find(tile);
                out_weights.insert_or_assign(tile, iterator != out_weights.end() ? iterator->second + 1 : 1);

                // check neighbours
                std::vector<Vector> valid_positions;
                const Vector current_position{i, j};
                valid_directions(current_position, width, height, valid_positions);
                for (const Vector& valid_position : valid_positions)
                {
                    const T* other_tile = matrix[i + valid_position.x][j + valid_position.y];
                    out_compatible_tiles.insert({tile, other_tile, valid_position});
                }
            }
        }
    }

    template <typename F>
    void run(std::set<CompatibleTile<const T*>>& compatible_tiles,
             WfcMap<T, int>& weights,
             const Vector& output_size,
             std::vector<std::vector<const T*>>& out_result, F iteration)
    {
        size = output_size;
        this->weights = weights;
        this->compatible_tiles = compatible_tiles;

        std::set<const T*> tiles;
        for (auto tileWeighed : weights)
        {
            tiles.insert(tileWeighed.first);
        }
        initialize_coefficients(output_size, tiles);
        while (!is_fully_collapsed())
        {
            iterate();
            iteration(coefficients);
        }

        get_all_collapsed(out_result);
    }

private:
    std::vector<std::vector<std::set<const T*>>> coefficients;
    WfcMap<T, int> weights;
    std::set<CompatibleTile<const T*>> compatible_tiles;
    Vector size;

    /**
    * Calculates the valid neighbour positions.
    * It takes the current_position and check up, down, left and right if there is a valid tile.
    */
    void valid_directions(const Vector& current_position, int width, int height,
                          OUT std::vector<Vector>& directions) const
    {
        if (current_position.x > 0) directions.push_back(Directions::left);
        if (current_position.x < width - 1) directions.push_back(Directions::right);
        if (current_position.y > 0) directions.push_back(Directions::down);
        if (current_position.y < height - 1) directions.push_back(Directions::up);
    }

    /**
     * Initializes a 2D matrix of coefficients. The matrix has a size "size" and each element of the matrix starts
     * with all tiles as possible. No tile is forbidden yet.
     *
     * @param size (width, height)
     * @param tiles a set of all the possible tiles
     */
    void initialize_coefficients(const Vector& size, const std::set<const T*>& tiles)
    {
        coefficients.clear();
        for (int i = 0; i < size.x; ++i)
        {
            std::vector<std::set<const T*>> row;
            for (int j = 0; j < size.y; ++j)
            {
                row.push_back(std::set<const T*>(tiles));
            }
            coefficients.push_back(row);
        }
    }

    /**
     *  Return true if every element in the coefficients is fully collapsed
     */
    bool is_fully_collapsed()
    {
        for (const auto& row : coefficients)
        {
            for (const auto& cell : row)
            {
                if (cell.size() > 1) return false;
            }
        }
        return true;
    }

    /**
     * Performs a single iteration of the algorithm
     */
    void iterate()
    {
        // 1. Find the co-ordinates of minimum entropy
        const Vector minimum_entropy = min_entropy();
        // 2. Collapse the wavefunction at these co-ordinates
        collapse(minimum_entropy);
        // 3. Propagate the consequences of this collapse
        propagate(minimum_entropy);
    }

    /**
     * Returns the coordinates of the location with the lowest entropy
     */
    Vector min_entropy()
    {
        float minimum_entropy = -1.0f;
        Vector minimum_entropy_cell{-1, -1};

        for (int i = 0; i < size.x; ++i)
        {
            for (int j = 0; j < size.y; ++j)
            {
                if (coefficients[i][j].size() == 1) continue;
                const Vector position{i, j};
                const float entropy = calculate_entropy(position) - Utils::get_random_number(0, 1) / 1000;
                if (minimum_entropy == -1 || entropy < minimum_entropy)
                {
                    minimum_entropy = entropy;
                    minimum_entropy_cell = position;
                }
            }
        }
        return minimum_entropy_cell;
    }

    /**
     * Calculates the Shannon entropy at the coordinates
     */
    float calculate_entropy(const Vector& coordinates)
    {
        int sum_of_weights = 0;
        float sum_of_log_weights = 0;

        for (const T* tile : coefficients[coordinates.x][coordinates.y])
        {
            if (!tile) continue;
            const int weight = weights[tile];
            sum_of_weights += weight;
            sum_of_log_weights += weight * std::log2(weight); // why is this multiplied by weight? I don't remember
        }
        return std::log2(sum_of_weights) - (sum_of_log_weights / sum_of_weights);
    }

    /**
     * Collapse the cell in the specific coordinates
     */
    void collapse(const Vector& coordinates)
    {
        std::set<const T*>& options = coefficients[coordinates.x][coordinates.y];
        WfcMap<T, int> current_weights;
        int total_weight = 0;
        for (const T* option : options)
        {
            const int weight = weights[option];
            total_weight += weight;
            current_weights.insert({option, weight});
        }
        float random_weight = Utils::get_random_number(0, total_weight);
        for (auto weight_option : current_weights)
        {
            random_weight -= weight_option.second;
            if (random_weight <= 0.0f)
            {
                options.clear();
                options.insert(weight_option.first);
                break;
            }
        }
    }

    /**
     * Propagates the consequences of the collapse of the cell at the coordinates "coordinates".
     * It keeps propagating the consequences of the consequences and so until no consequences remain.
     */
    void propagate(const Vector& coordinates)
    {
        std::stack<Vector> stack;
        stack.push(coordinates);
        while (stack.size() > 0)
        {
            const Vector current_coordinates = stack.top();
            stack.pop();
            const std::set<const T*>& options = coefficients[current_coordinates.x][current_coordinates.y];

            // iterate through each location immediately adjacent to the current location
            std::vector<Vector> neighbour_directions;
            valid_directions(current_coordinates, size.x, size.y, neighbour_directions);
            for (const Vector& direction : neighbour_directions)
            {
                const Vector neighbour = {current_coordinates.x + direction.x, current_coordinates.y + direction.y};
                std::set<const T*>& neighbour_options = coefficients[neighbour.x][neighbour.y];
                for (auto iter = neighbour_options.begin(); iter != neighbour_options.end();)
                {
                    if (!has_compatible_tile(options, *iter, direction))
                    {
                        iter = neighbour_options.erase(iter);
                        stack.push(neighbour);
                    }
                    else
                    {
                        ++iter;
                    }
                }
                assert(neighbour_options.size() > 0);
            }
        }
    }


    bool has_compatible_tile(const std::set<const T*>& options, const T* neighbour_option, const Vector& direction)
    {
        for (CompatibleTile<const T*> compatible_tile : compatible_tiles)
        {
            for (const T* option : options)
            {
                if (!option) continue;
                if (*option == *compatible_tile.tile1 &&
                    *neighbour_option == *compatible_tile.tile2 &&
                    compatible_tile.direction == direction)
                {
                    return true;
                }
            }
        }
        return false;
    }

    void get_all_collapsed(std::vector<std::vector<const T*>>& out_collapsed)
    {
        for (auto row : coefficients)
        {
            std::vector<const T*> row_collapsed;
            for (std::set<const T*> cell_options : row)
            {
                row_collapsed.push_back(*cell_options.begin());
            }
            out_collapsed.push_back(row_collapsed);
        }
    }
};

