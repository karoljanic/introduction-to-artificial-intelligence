#ifndef STATE_H
#define STATE_H

#include <iostream>
#include <vector>

typedef unsigned long int ull;

class State {
public:
    static ull onePuzzleMask;
    static short size;

    ull representation;

    ull gValue;
    ull hValue;

    State* previousStage;
    bool isOnOpenList;
    size_t heapIdx;
    size_t hashValue;

    State();
    State(ull stateValue);
    State(const std::vector<short>& puzzlesOrder);

    short getEmptyPuzzelIndex() const;
    std::vector<ull> generateNeighbors() const;

    short inversionNumber() const;
    short misplacedTiles() const;
    short manhattanDistance() const;
    short invertDistance() const;

    friend bool operator<(const State& state1, const State& state2);
    friend bool operator>(const State& state1, const State& state2);
    friend bool operator==(const State& state1, const State& state2);

    friend std::ostream& operator<<(std::ostream& stream, const State& state);

    short at(short index) const;
    ull swap(short index1, short index2) const;
    void setHeuristic();
    void setHash();
};


#endif  // STATE_H