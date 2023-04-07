#include "State.h"

short State::size = 4;
ull State::onePuzzleMask = 0xF;

State::State() :
    representation{0x0FEDCBA987654321},
    gValue{0},
    hValue{0},
    previousStage{nullptr},
    isOnOpenList{false},
    heapIdx{0},
    hashValue{0} 
{ 
    setHeuristic();
    setHash();
}

State::State(ull stateValue) : 
    representation{stateValue}, 
    gValue{0},
    hValue{0},
    previousStage{nullptr}, 
    isOnOpenList{false},
    heapIdx{0},
    hashValue{0} 
{ 
    setHeuristic();
    setHash();
}

State::State(const std::vector<short>& puzzlesOrder) : 
    representation{0},
    gValue{0},
    hValue{0},
    previousStage{nullptr}, 
    isOnOpenList{false},
    heapIdx{0},
    hashValue{0}
{

    for(short index = 0; index < 16; index++) {
        representation |= ((onePuzzleMask & puzzlesOrder[index]) << (index << 2));
    }

    setHeuristic();
    setHash();
}

short State::getEmptyPuzzelIndex() const {
    for(short index = 0; index < 16; index++) {
        ull mask = onePuzzleMask << 4 * index;
            
        if(((representation & mask) >> 4 * index) == 0) {
            return index;
        }
    }

    return -1;
}

std::vector<ull> State::generateNeighbors() const {
    short basePosition = getEmptyPuzzelIndex();
    short row = basePosition / 4;
    short column = basePosition % 4;

    std::vector<ull> neighbors;
    if(row > 0) {
        neighbors.emplace_back(swap(basePosition, basePosition - size));
    }

    if(row < size - 1) {
        neighbors.emplace_back(swap(basePosition, basePosition + size));
    }

    if(column > 0) {
        neighbors.emplace_back(swap(basePosition, basePosition - 1));
    }

    if(column < size - 1) {
        neighbors.emplace_back(swap(basePosition, basePosition + 1));
    }

    return neighbors;
}

short State::inversionNumber() const {
    short counter = 0;

    for (short i = 0; i < 15; i++) {
        for (short j = i + 1; j < 16; j++) {

            short pi = static_cast<short>(representation & (onePuzzleMask << (i << 2))) >> (i << 2);
            short pj = static_cast<short>(representation & (onePuzzleMask << (j << 2))) >> (j << 2);

            if (pi != 0 && pj != 0 && pi > pj) {
                counter++;
            }
        }
    }

    return counter;
}

short State::misplacedTiles() const {
    short misplacements = 0;

    for(short index = 0; index < 15; index++) {
        short val = at(index) - 1;
        if(val != index) {
            misplacements++;
        }
    }

    return misplacements;
}

short State::manhattanDistance() const {
    short sumOfDistances = 0;
    for(short index = 0; index < 16; index++) {
        short val = at(index) - 1;
        if(val != -1) {
            sumOfDistances += static_cast<short>(std::abs(val / 4 - index / 4));
            sumOfDistances += static_cast<short>(std::abs(val % 4 - index % 4));
        }
    }

    return sumOfDistances;
}

short State::invertDistance() const {
    std::vector<short> board;
    board.reserve(16);
    for(short i = 0; i < 16; i++) {
        board.push_back(at(i));
    }

    short horizontal = 0;
    for(short i = 0; i < 16; i++) {
        short num1 = board[i];
        if (num1 == 0) {
            continue;
        }

        for(short j = i + 1; j < 16; j++) {
            short num2 = board[j];

            if (num2 != 0 && num2 < num1) {
                horizontal++;
            }
        }
    }

    std::vector<short> boardTransLookUp { 0, 1, 5, 9, 13, 2, 6, 10, 14, 3, 7, 11, 15, 4, 8, 12 };
    std::vector<short> indexTransLookUp{ 0, 4, 8,12, 1, 5, 9,13, 2, 6, 10, 14, 3, 7, 11, 15};

    short vertical = 0;
    for (short i = 0; i < 16; i++) {
        short num1 = boardTransLookUp[board[indexTransLookUp[i]]];
        if (!num1) continue;
        for (short j = i + 1; j < 16; j++) {
            short num2 = boardTransLookUp[board[indexTransLookUp[j]]];
            if (num2 != 0 && num2 < num1) {
                vertical++;
            }
        }
    }

    short sumOfDistances = 0;
    sumOfDistances += horizontal / 3;
    sumOfDistances += horizontal % 3;
    sumOfDistances += vertical / 3;
    sumOfDistances += vertical % 3;

    return sumOfDistances;
}

bool operator<(const State& state1, const State& state2) {
    if ((state1.gValue + state1.hValue) < (state2.gValue + state2.hValue)) {
        return true;
    }
    
    if ((state1.gValue + state1.hValue) == (state2.gValue + state2.hValue) && (state1.hValue < state2.hValue)) {
        return true;
    }
    
    return false;
}

bool operator>(const State& state1, const State& state2) {
    return state2 < state1;
}

bool operator==(const State& state1, const State& state2) {
    return state2.representation == state1.representation;
}

std::ostream& operator<< (std::ostream& stream, const State& state) {    
    for(short row = 0; row < State::size; row++) {
        for(short column = 0; column < State::size; column++) {
            short shiftSize = static_cast<short>((State::size * row + column) << 2);
            ull mask = State::onePuzzleMask << shiftSize;
            std::cout << std::hex << ((state.representation & mask) >> shiftSize) << std::dec << ' ';
        }
        std::cout << '\n';
    }

   return stream;
}

short State::at(short index) const {    
    return static_cast<short>((representation & (State::onePuzzleMask << (index << 2))) >> (index << 2));
}

ull State::swap(short index1, short index2) const {
    ull state = representation;

    ull mask1 = (onePuzzleMask << (index1 << 2));
    ull invertedMask1 = ~mask1;
    ull mask2 = (onePuzzleMask << (index2 << 2));
    ull invertedMask2 = ~mask2;

    ull element1;
    ull element2;

    if(index1 > index2) {
        element1 = (state & mask1) >> ((index1 - index2) << 2);
        element2 = (state & mask2) << ((index1 - index2) << 2);
    }
    else {
        element1 = (state & mask1) << ((index2 - index1) << 2);
        element2 = (state & mask2) >> ((index2 - index1) << 2);
    }

    state &= invertedMask1;
    state &= invertedMask2;

    return state | element1 | element2;
}

void State::setHeuristic() {
    //hValue = manhattanDistance();
    //hValue = invertDistance();
    hValue = std::max(manhattanDistance(), invertDistance());
}

void State::setHash() {
    hashValue = std::hash<ull>{}(representation);
}