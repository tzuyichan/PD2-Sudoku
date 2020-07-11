//
//  transform.cpp
//  pd2-sudoku
//
//  Created by Ethan Chan on 2020/4/2.
//  Copyright © 2020 Ethan Chan. All rights reserved.
//

#include <iostream>
#include <vector>
#include <set>
#include "sudoku.h"

int main() {
//user input sudoku problem
    int input[9][9];
    for (int i=0; i<9; ++i) {
        for (int j=0; j<9; ++j) {
            std::cin >> input[i][j];
        }
    }

//initialize new sudoku object with input
    Sudoku sudoku1(*input);

//transform sudoku with functions operations
    int mode, x, y;
    do {
        std::cin >> mode;
        switch (mode) {
        case 1:
            std::cin >> x >> y;
            sudoku1.swapNum(x, y);
            break;
        case 2:
            std::cin >> x >> y;
            sudoku1.swapRow(x, y);
            break;
        case 3:
            std::cin >> x >> y;
            sudoku1.swapCol(x, y);
            break;
        case 4:
            std::cin >> x;
            sudoku1.rotate(x);
            break;
        case 5:
            std::cin >> x;
            sudoku1.flip(x);
            break;
        }
    } while (mode);

//output transformed sudoku
    int* current_map = sudoku1.getMap();
    for (int i=0; i<9; ++i) {
        for (int j=0; j<9; ++j) {
            std::cout << current_map[i*9+j];
            if (j == 8) {
                std::cout << '\n';
            } else {
                std::cout << ' ';
            }
        }
    }

}
