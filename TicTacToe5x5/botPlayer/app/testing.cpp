#include <iostream>

#include "Bot.hpp"

#ifdef TEST

int main() {
    State boardStates[5][5] = {
        {State::EMPTY, State::BOT_MARKER, State::EMPTY, State::EMPTY, State::EMPTY},
        {State::EMPTY, State::EMPTY, State::EMPTY, State::EMPTY, State::EMPTY},
        {State::EMPTY, State::BOT_MARKER, State::EMPTY, State::EMPTY, State::EMPTY},
        {State::EMPTY, State::EMPTY, State::EMPTY, State::EMPTY, State::EMPTY},
        {State::EMPTY, State::EMPTY, State::EMPTY, State::EMPTY, State::EMPTY}
    };

    Board board{boardStates, {2, 2, 2, 2, 2, 2, 2}};
    board.display(std::cout);

    HybridHeuristicComponents components = board.calculateHybridHeuristicComponentsForPlayer(State::BOT_MARKER);

    std::cout << "movementFreedom = " << components.movementFreedom << std::endl;
    std::cout << "winLosePositions = " << components.winLosePositions << std::endl;
    std::cout << "doubleMarkerZeroEmptyBetween = " << components.doubleMarkerZeroEmptyBetween << std::endl;
    std::cout << "doubleMarkerOneEmptyBetween = " << components.doubleMarkerOneEmptyBetween << std::endl;
    std::cout << "doubleMarkerTwoEmptyBetween = " << components.doubleMarkerTwoEmptyBetween << std::endl;
    std::cout << "tripleMarkerOneEmptyBetween = " << components.tripleMarkerOneEmptyBetween << std::endl;
    std::cout << "tripleMarkerTwoEmptyBetween = " << components.tripleMarkerTwoEmptyBetween << std::endl;

    
    std::vector<int> moves = board.movesToWin(State::BOT_MARKER);
    std::cout << "Moves count = " << moves.size() << '\n';
    std::cout << "Moves: ";
    for(const int& move: moves) {
        std::cout << move << " ";
    }
    std::cout << '\n';
    

    return 0;
}

#endif // TEST