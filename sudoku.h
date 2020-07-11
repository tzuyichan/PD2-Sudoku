#ifndef SUDOKU_H
#define SUDOKU_H

/***************************************************
 * Finish your .cpp according to this header file. *
 * You can modify this file if needed.             *
 ***************************************************/

class Sudoku {
  public:
    // constructor
    Sudoku();
    Sudoku(const int init_map[]);

    // basic attributes
    static const int sudokuSize = 81;

    // transform
    void swapNum(const int x, const int y);
    void swapRow(const int x, const int y);
    void swapCol(const int x, const int y);
    void rotate(const int x);
    void flip(const int x);

    // solve
    bool solve(Sudoku question, Sudoku& answer);
    bool fillNakedSingle();
    bool fillHiddenSingle();
    bool findNakedTuple();

    // others
    void printMap();
    int* getMap();
    int getElement(const int index);
    const int getNumOfGivens();
    int getFirstZeroIndex();
    void getLeastCandidateBlank();
    void updateZeroPositions(); // updates zero_posits
    bool isCorrect();
    bool checkUnity(const int arr[]);
    void checkUnusedV(const int index, std::vector<int>& v_unused); // return valid options per cell
    void checkUnusedS(const int index, std::set<int>& s_unused);
    static void initializeHouseIterator(); // generates iterator for the 27 houses
    void initializeCddtMap(); // generate a map of all the possible options for each cell

  private:
    int map[sudokuSize];
    int init_map[sudokuSize];
    static int numOfGivens;
    static int numOfBlanks;
    std::vector<int> zero_posits; // vector of blank indices in numerical order
    static std::vector<std::set<int> > cddt_map; // candidate_map
    int leastCandidateBlankNum;
    static int recursion_depth;
    static std::vector<std::vector<std::vector<int> > > house_it; // 2D vector of the 27 houses
    void setMap(const int set_map[]);
    void setElement(const int index, const int value);
    void eraseCddtInNeighbors(const int index, const int val);
};

#endif // SUDOKU_H
