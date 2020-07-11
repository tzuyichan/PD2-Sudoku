//
//  sudoku.cpp
//  pd2-sudoku
//
//  Created by Ethan Chan on 2020/4/2.
//  Copyright © 2020 Ethan Chan. All rights reserved.
//

#include <iostream>
#include <vector>
#include <set>
#include <algorithm>
#include <math.h>
#include <cstring>
#include "sudoku.h"

int Sudoku::numOfGivens;
int Sudoku::numOfBlanks;
int Sudoku::recursion_depth = 0;
std::vector<std::vector<std::vector<int> > > Sudoku::house_it;
std::vector<std::set<int> > Sudoku::cddt_map; // records the candidates for each cell in a map


// constructor
Sudoku::Sudoku() {
    int zeros[sudokuSize] = {0};
    setMap(zeros);
    for (int i=0; i<sudokuSize; ++i) {
        init_map[i] = map[i];
    }
    numOfGivens = getNumOfGivens(); // count number of inital givens
    numOfBlanks = sudokuSize - numOfGivens; // count number of initial blank cells
    getLeastCandidateBlank();
}


Sudoku::Sudoku(const int init_map[]) {
    setMap(init_map);
    for (int i=0; i<sudokuSize; ++i) {
        Sudoku::init_map[i] = map[i];
    }
    numOfGivens = getNumOfGivens(); // count number of inital givens
    numOfBlanks = sudokuSize - numOfGivens; // count number of initial blank cells
    getLeastCandidateBlank();
}


// transform
void Sudoku::swapNum(const int x, const int y) { // swap x with y
    for (int i=0; i<sudokuSize; ++i) {
        if (map[i] == x) {
            map[i] = y;
        } else if (map[i] == y) {
            map[i] = x;
        }
    }
}


void Sudoku::swapRow(const int x, const int y) { // swap band (3 rows) x with y
    for (int i=0; i<sudokuSize/3; ++i) {
        int temp = map[x*(sudokuSize/3)+i];
        map[x*(sudokuSize/3)+i] = map[y*(sudokuSize/3)+i];
        map[y*(sudokuSize/3)+i] = temp;
    }
}


void Sudoku::swapCol(const int x, const int y) {  // swap stack (3 columns) x with y
    int width = sqrt(sudokuSize);
    for (int i=0; i<width; ++i) {
        for (int j=0; j<3; ++j) {
            int temp = map[i*width+x*3+j];
            map[i*width+x*3+j] = map[i*width+y*3+j];
            map[i*width+y*3+j] = temp;
        }
    }
}


void Sudoku::rotate(const int x) { // rotate x times in the clockwise direction
    int width = sqrt(sudokuSize);
    for (int rep=0; rep<x; ++rep) {
        for (int i=0; i<width/2; ++i) {
            for (int j=i; j<width-i-1; ++j) {
                int temp = map[i*width+j];
                map[i*width+j] = map[(width-1-j)*width+i];
                map[(width-1-j)*width+i] = map[(width-1-i)*width+(width-1-j)];
                map[(width-1-i)*width+(width-1-j)] = map[j*width+(width-1-i)];
                map[j*width+(width-1-i)] = temp;
            }
        }
    }
}


void Sudoku::flip(const int x) { // flip w.r.t center axis
    int width = sqrt(sudokuSize);
    switch (x) {
    case 0: // up down flip
        for (int i=0; i<floor(width/2); ++i) {
            for (int j=0; j<width; ++j) {
                int temp = map[i*width+j];
                map[i*width+j] = map[(width-1-i)*width+j];
                map[(width-1-i)*width+j] = temp;
            }
        }
        break;
    case 1: // left right flip
        for (int i=0; i<width; ++i) {
            for (int j=0; j<floor(width/2); ++j) {
                int temp = map[i*width+j];
                map[i*width+j] = map[i*width+(width-1-j)];
                map[i*width+(width-1-j)] = temp;
            }
        }
        break;
    }
}


// solve
bool Sudoku::solve(Sudoku question, Sudoku& answer) {
    int firstZero;
    if (recursion_depth < 1)
        firstZero = leastCandidateBlankNum;
    else
        firstZero = question.getFirstZeroIndex();
    if (firstZero == -1) {
        if (question.isCorrect()) {
            answer = question;
            return true;
        } else
            return false;
    } else {
        Sudoku::recursion_depth++;
        std::vector<int> v_unused;
        question.checkUnusedV(firstZero, v_unused);
        for (int i=0; i<v_unused.size(); ++i) {
            question.setElement(firstZero, v_unused[i]);
            if (question.solve(question, answer)) //recursion
                return true;
        }
        Sudoku::recursion_depth--;
        return false;
    }
}


bool Sudoku::fillNakedSingle() {
    int count = 0;
    std::set<int>::iterator it;
    for (int i=0; i<sudokuSize; ++i) {
        if (cddt_map[i].size() == 1) {
            it = cddt_map[i].begin();
            map[i] = *it; // fill in naked single
            eraseCddtInNeighbors(i, *it); // update neighbor candidates
            count = 1;
        }
    }
    return count;
}


bool Sudoku::fillHiddenSingle() {
    int found_hidden_single = 0;
    std::set<int>::iterator it;
    for (int i=0; i<3; ++i) { // row = 0; col = 1; block = 2
        for (int j=0; j<9; ++j) { // scan each row, col or block
            int arr_unity[9] = {0};
            for (int k=0; k<9; ++k) { // scan each element
                // scan and extract
                if (cddt_map[house_it[i][j][k]].size() != 0) { // if blank is not filled in
                    for (it=cddt_map[house_it[i][j][k]].begin(); it!=cddt_map[house_it[i][j][k]].end(); ++it) {
                        ++arr_unity[*it-1]; // count number of appearances of candidates
                    }
                }
            }
            // find hidden single
            for (int n=0; n<9; ++n) {
                if (arr_unity[n] == 1) { // if a candidate num only appeared once in this row
                    int hidden_single = n + 1; // found hidden single
                    for (int k=0; k<9; ++k) { // find hidden single belongs to which blank
                        if (cddt_map[house_it[i][j][k]].count(hidden_single) == 1) {
                            map[house_it[i][j][k]] = hidden_single; // set blank to hidden single
                            cddt_map[house_it[i][j][k]].clear(); // clear set in candidate map
                            eraseCddtInNeighbors(house_it[i][j][k], hidden_single); // update neighbors
                            found_hidden_single = 1;
                        }
                    }
                }
            }
        }
    }
    return found_hidden_single;
}


bool Sudoku::findNakedTuple() {
    int found_naked_tuple = 0;
    std::set<int>::iterator it;
    for (int num=2; num<=4; ++num) { // search naked pair, triple and quad
        for (int i=0; i<3; ++i) { // row = 0; col = 1; block = 2
            for (int j=0; j<9; ++j) { // scan each row, col or block
                for (int k=0; k<9; ++k) { // scan each element
                    std::set<int> set_unity = {};
                    if (cddt_map[house_it[i][j][k]].size() == num) { // if blank has num candidates
                        set_unity.emplace(k); // record position
                        for (int l=k+1; l<9; ++l) { // search blanks with identical candidates
                            if (cddt_map[house_it[i][j][k]] == cddt_map[house_it[i][j][l]])
                                set_unity.emplace(l); // record position
                        }
                        if (set_unity.size() == num) { // if found num of identical blanks
                            for (int m=0; m<9; ++m) { // iterate elements in house_it[i][j]
                                if (set_unity.count(m) == 0) { // if not blank of hidden tuple
                                    for (it=cddt_map[house_it[i][j][k]].begin(); it!=cddt_map[house_it[i][j][k]].end(); ++it) {
                                        cddt_map[house_it[i][j][m]].erase(*it); // erase hidden tuple candidates from blank
                                    }
                                }
                            }
                            found_naked_tuple = 1;
                        }
                    }
                }
            }
        }
    }
    return found_naked_tuple;
}


// others
bool Sudoku::isCorrect() {
    bool check_result;
    int check_arr[9];
    int location;
    for (int i=0; i<81; i+=9) { // check rows
        for (int j=0; j<9; ++j) {
            check_arr[j] = map[i+j];
        }
        check_result = checkUnity(check_arr);
        if (check_result == false)
            return false;
    }
    for (int i=0; i<9; ++i) { // check columns
        for (int j=0; j<9; ++j) {
            check_arr[j] = map[i+9*j];
        }
        check_result = checkUnity(check_arr);
        if (check_result == false)
            return false;
    }
    for (int i=0; i<9; ++i) { // check blocks
        for (int j=0; j<9; ++j) {
            location = 27*(i/3) + 3*(i%3) + 9*(j/3) + (j%3);
            check_arr[j] = map[location];
        }
        check_result = checkUnity(check_arr);
        if (check_result == false)
            return false;
    }
    return true;
}


void Sudoku::setMap(const int set_map[]) {
    for (int i=0; i<sudokuSize; ++i) {
        map[i] = set_map[i];
    }
}


int* Sudoku::getMap() {
    return map;
}


void Sudoku::setElement(const int index, const int value) {
    map[index] = value;
}


int Sudoku::getElement(const int index) {
    return map[index];
}


const int Sudoku::getNumOfGivens() {
    int count = 0;
    for (int i=0; i<sudokuSize; ++i) {
        if (init_map[i] != 0)
            count++;
    }
    return count;
}


int Sudoku::getFirstZeroIndex() {
    for (int i=0; i<sudokuSize; ++i) {
        if (map[i] == 0)
            return i;
    }
    return -1;
}


void Sudoku::updateZeroPositions() {
    zero_posits.clear();
    int idx = 0;
    for (int i=0; i<sudokuSize; ++i) {
        if (map[i] == 0) {
            zero_posits.emplace_back(i);
            idx++;
        }
    }
}


void Sudoku::getLeastCandidateBlank() {
    int leastCandidateNum = 9;
    updateZeroPositions(); // initialize vector::zero_posits
    for (int i=0; i<zero_posits.size(); ++i) {
        std::set<int> s_unused;
        checkUnusedS(zero_posits[i], s_unused);
        if (s_unused.size() < leastCandidateNum) {
            leastCandidateNum = int(s_unused.size());
            leastCandidateBlankNum = zero_posits[i];
        }
    }
}


void Sudoku::printMap() {
    for (int i=0; i<9; ++i) {
        for (int j=0; j<9; ++j) {
            std::cout << map[i*9+j];
            if (j == 8) {
                std::cout << '\n';
            } else {
                std::cout << ' ';
            }
        }
    }
}


bool Sudoku::checkUnity(const int arr[]) { // pass in a row, col or block arr[9] to check
    int arr_unity[9] = {0}; // counter array, goal is to make arr_unity = {1}
    for (int i=0; i<9; ++i) {
        ++arr_unity[arr[i]-1]; // count, eg arr = [1 4 3 4] --> arr_unity = [1 0 1 2]
    }
    for (int i=0; i<9; ++i) {
        if (arr_unity[i] != 1) // all element
            return false; // must be 1
    }
    return true;
}


void Sudoku::checkUnusedV(const int index, std::vector<int>& v_unused) {
    int arr_unity[9] = {0}; // counter array, arr_unity[i-1]++ for each i (i = 1~9) existing in corresponding row, col or blk of map[index]
    int blkNum = (index/27)*3 + (index%9)/3; // blkNum between 1 to 9

    for (int i=0; i<9; ++i) { // count used nums in row
        if (map[(index/9)*9+i] != 0)
            ++arr_unity[map[(index/9)*9+i]-1];
    }
    for (int i=0; i<sudokuSize; i+=9) { // count used nums in col
        if (map[i+(index%9)] != 0)
            ++arr_unity[map[i+(index%9)]-1];
    }
    for (int i=0; i<9; ++i) { // count used nums in blk
        int location = 27*(blkNum/3) + 3*(blkNum%3) + 9*(i/3) + (i%3); // element index in map[]
        if (map[location] != 0)
            ++arr_unity[map[location]-1];
    }

    for (int i=0; i<9; ++i) { // return vector of valid (unused) elements in map[index]
        if (arr_unity[i] == 0)
            v_unused.emplace_back(i+1);
    }
}


void Sudoku::checkUnusedS(const int index, std::set<int>& s_unused) {
    int arr_unity[9] = {0}; // counter array, arr_unity[i-1]++ for each i (i = 1~9) existing in corresponding row, col or blk of map[index]
    int blkNum = (index/27)*3 + (index%9)/3; // blkNum between 1 to 9

    for (int i=0; i<9; ++i) { // count used nums in row
        if (map[(index/9)*9+i] != 0)
            ++arr_unity[map[(index/9)*9+i]-1];
    }
    for (int i=0; i<sudokuSize; i+=9) { // count used nums in col
        if (map[i+(index%9)] != 0)
            ++arr_unity[map[i+(index%9)]-1];
    }
    for (int i=0; i<9; ++i) { // count used nums in blk
        int location = 27*(blkNum/3) + 3*(blkNum%3) + 9*(i/3) + (i%3); // element index in map[]
        if (map[location] != 0)
            ++arr_unity[map[location]-1];
    }

    for (int i=0; i<9; ++i) { // return vector of valid (unused) elements in map[index]
        if (arr_unity[i] == 0)
            s_unused.emplace(i+1);
    }
}


void Sudoku::eraseCddtInNeighbors(const int index, const int val) {
    int blkNum = (index/27)*3 + (index%9)/3; // blkNum between 1 to 9
    for (int i=0; i<9; ++i)
        cddt_map[(index/9)*9+i].erase(val); // erase val in neighbor row elements
    for (int i=0; i<sudokuSize; i+=9)
        cddt_map[i+(index%9)].erase(val); // erase val in neighbor col elements
    for (int i=0; i<9; ++i) {
        int location = 27*(blkNum/3) + 3*(blkNum%3) + 9*(i/3) + (i%3); // element index in map[]
        cddt_map[location].erase(val); // erase val in neighbor block elements
    }
}


void Sudoku::initializeHouseIterator() {
    std::vector<std::vector<int> > all_rows, all_cols, all_blocks;
    std::vector<int> row, col, block;
    for (int i=0; i<9; ++i) {
        for (int j=0; j<9; ++j) {
            row.emplace_back(i*9+j);
            col.emplace_back(i+9*j);
            int location = 27*(i/3) + 3*(i%3) + 9*(j/3) + (j%3);
            block.emplace_back(location);
        }
        all_rows.push_back(row);
        all_cols.push_back(col);
        all_blocks.push_back(block);
        row.clear();
        col.clear();
        block.clear();
    }
    house_it.push_back(all_rows);
    house_it.push_back(all_cols);
    house_it.push_back(all_blocks);
}


void Sudoku::initializeCddtMap() {
    std::set<int> temp;
    cddt_map.resize(sudokuSize, temp);
    for (int i=0; i<zero_posits.size(); ++i) {
        checkUnusedS(zero_posits[i], cddt_map[zero_posits[i]]);
    }
}

