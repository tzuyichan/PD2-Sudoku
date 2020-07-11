//
//  solve.cpp
//  pd2-sudoku
//
//  Created by Ethan Chan on 2020/4/2.
//  Copyright © 2020 Ethan Chan. All rights reserved.
//

#include <stdio.h>
#include <iostream>
#include <vector>
#include <set>
#include "sudoku.h"
#include <algorithm>
#include <iterator>


int main() {
// input sudoku problem
    int input[9][9];
    for (int i=0; i<9; ++i) {
        for (int j=0; j<9; ++j) {
            std::cin >> input[i][j];
        }
    }

// initialize new sudoku object with input
    Sudoku question(*input);
    Sudoku answer;

// solve sudoku problem
    if (question.getNumOfGivens() < 17) // question has multiple solutions
        std::cout << 2 <<std::endl;
    else {
        Sudoku::initializeHouseIterator(); // iterator for enumerating the 27 houses
        question.initializeCddtMap(); // candidate list for each blank, result is a 9x9 2D vector
        int count = 0;

        // rule-based elimination
        while (true) {
            if (question.fillNakedSingle()) {
                count = 0;
                continue;
            }
            if (question.fillHiddenSingle()) {
                count = 0;
                continue;
            }
            if (question.findNakedTuple() && count == 0) {
                count = 1;
                continue;
            }
            question.getLeastCandidateBlank();
            break;
        }
        if (question.isCorrect()) {
            std::cout << 1 << std::endl;
            answer = question;
            answer.printMap();
            return 0; // mission accomplished, program ends here
        }

        // recursion backtracking
        if (question.solve(question, answer) == true) { // question has unique solution
            std::cout << 1 << std::endl;
            answer.printMap();
        } else // question has no solution
            std::cout << 0 << std::endl;
    }

    return 0;
}
