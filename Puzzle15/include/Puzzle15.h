#ifndef PUZZLE_H
#define PUZZLE_H

#include <queue>
#include <set>
#include <algorithm>
#include <random>

#include "State.h"
#include "PriorityQueue.h"
#include "UnorderedSet.h"


class Puzzle15 {
private:
    State startState;

    std::mt19937 generator;

public:
    Puzzle15();
    Puzzle15(ull initialState);

    bool canBeSolved() const;
    void shuffle(short swapsNumber);
    void randomShuffle();
    void findSolution();

    friend std::ostream& operator<<(std::ostream& stream, const Puzzle15& puzzle);
};

#endif  // PUZZLE_H