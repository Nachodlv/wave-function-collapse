#pragma once
#include <iostream>
#include <set>
#include <vector>
#include <vector>
#include <windows.h>


class Tile;

enum ConsoleColors
{
    normal = 7,
    blue = 16,
    green = 35,
    blue_green = 56,
    red = 72,
    yellow = 104,
    gray = 128,
    black = 134
};

class IPrintable
{
public:
    virtual ~IPrintable() {}
    virtual char get_character() const = 0;
    virtual ConsoleColors get_color() const = 0;
};

class Printer
{
public:
    Printer();

    /**
     * Prints the matrix using the color of the Tile
     */
    template <typename T>
    void print_matrix(const std::vector<std::vector<T>>& matrix_to_print, const std::string space_between_columns = "\t") const
    {
        for (int i = 0; i < matrix_to_print.size(); i++)
        {
            for (int k = 0; k < matrix_to_print[i].size(); k++)
            {
                if (T printable = matrix_to_print[i][k])
                {
                    SetConsoleTextAttribute(h_console, printable->get_color());
                    std::cout << printable->get_character() << space_between_columns;
                }
            }
            std::cout << std::endl;
        }
        SetConsoleTextAttribute(h_console, normal);
    }

    template<typename T>
    void print_non_collapsed_matrix(const std::vector<std::vector<std::set<T>>>& matrix_to_print) const
    {
        const int space = get_space_between_characters(matrix_to_print);
        for (auto row : matrix_to_print)
        {
            for (auto options : row)
            {
                for (T printable : options)
                {
                    if (!printable) continue;
                    std::cout << printable->get_character();
                }
                for (int i = 0; i < space - options.size(); ++i)
                {
                    std::cout << " ";
                }
                std::cout << " ";
            }
            std::cout << std::endl;
        }
        SetConsoleTextAttribute(h_console, normal);
    }

    /**
     * Prints all the possible colors that can be used and their codes.
     */
    void test_colors() const;

private:
    HANDLE h_console;

    template<typename T>
    int get_space_between_characters(const std::vector<std::vector<std::set<T>>>& matrix) const
    {
        int max_options_quantity = 0;
        for (auto row : matrix)
        {
            for (auto options : row)
            {
                if (options.size() > max_options_quantity) max_options_quantity = options.size();
            }
        }
        return max_options_quantity;
    }
};

