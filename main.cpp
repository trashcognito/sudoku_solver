#include <cstdlib>
#include <ios>
#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include "sudoku.hpp"

void load_puzzle(std::istream &src, std::vector<std::vector<int>> &puzzle) {
    std::string row;
    int x=0, y=0;
    while (std::getline(src, row)) {
        std::stringstream n(row);
        std::string single_num;
        while (std::getline(n, single_num, ',')) {
            puzzle[y][x] = std::atoi(single_num.c_str());
            x++; 
        }
        x=0;
        y++;
    };
}

int main(int, char**) {
    std::vector<std::vector<int>> puzzle;
    for (int i=0;i<9;i++) {
        std::vector<int> temp;
        temp.resize(9);
        puzzle.push_back(temp);
    }

    std::string input_csv;
    auto file = std::fstream("input.csv", std::ios_base::in);
    load_puzzle(file, puzzle);
    file.close();

    Sudoku sudoku(puzzle);

    GroupSpeculateRule r_s;
    OnlyPossibilityRule r_1;
    RowRule r_2;
    ColumnRule r_3;
    GroupRule r_4;

    sudoku.bind_rule(&r_s);
    sudoku.bind_rule(&r_1);
    sudoku.bind_rule(&r_2);
    sudoku.bind_rule(&r_3);
    sudoku.bind_rule(&r_4);

    sudoku.solve();

    auto solved = sudoku.get();

    for (int col=0;col<9;col++) {
        for (int row=0;row<9;row++) {
            if (solved[col][row] != 0) {
                std::cout << solved[col][row];
            } else {
                std::cout << "_";
            }
            
            if (row != 8) {
                if (row % 3 == 2) {
                    std::cout << "|";
                } else {
                    std::cout << " ";
                }
            }
        }
        std::cout << std::endl;
        if (col != 8) {
            if (col % 3 == 2) {
                std::cout << "-----+-----+-----";
            } else {
                std::cout << "     |     |     ";
            }
            std::cout << std::endl;

        }
    }
    
    return 0;
}
