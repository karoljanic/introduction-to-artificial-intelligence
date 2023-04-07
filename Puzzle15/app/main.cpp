#include <iostream>

#include "Puzzle15.h"

int main(int /*argc*/, char** /*argv*/) {
    
    Puzzle15 puzzle15{};
    bool canBeSolved = false;
    while(!canBeSolved) {
        puzzle15.randomShuffle();
        canBeSolved = puzzle15.canBeSolved();
    }
    

    //Puzzle15 puzzle15{};
    //Puzzle15 puzzle15{0x08ce532dba174f69};
    //Puzzle15 puzzle15{0x123456789abcdef0};
    

    std::cout << puzzle15 << '\n';
    puzzle15.findSolution();    

    return 0;
}
