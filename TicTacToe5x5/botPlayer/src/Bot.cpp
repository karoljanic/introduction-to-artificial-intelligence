#include "Bot.hpp"

Bot::Bot(unsigned short maximalDepth, HeuristicType heuristicType, bool hasFirstMove): 
    depth{maximalDepth}, 
    heuristic{heuristicType},
    starter{hasFirstMove},
    board{},
    mersenneTwister{time(0)} {

        if(hasFirstMove) {
            botRole = State::PLAYER_X;
            opponentRole = State::PLAYER_O;
        }
        else {
            botRole = State::PLAYER_O;
            opponentRole = State::PLAYER_X;
        }
    }

int Bot::opponentMove(unsigned short row, unsigned short column) {
    board.nextMove(row, column, opponentRole);


    board.display(std::cout);
    return 0;
}

int Bot::move() {
    
    //board.nextMove(move / 10, move % 10, botRole);
    board.display(std::cout);

    return 0;
}

int Bot::randomMove() {
    std::vector<int> possibleMoves{board.getPossibleMoves()};
    std::uniform_int_distribution<unsigned short> distribution{0, possibleMoves.size() - 1};

    int move = possibleMoves[distribution(mersenneTwister)];

    board.nextMove(move / 10, move % 10, botRole);
    board.display(std::cout);

    return move;
}

int Bot::minMax(State player, unsigned short depth, unsigned short* bestX, unsigned short* bestY) {
    
}

State Bot::getOppositePlayer(State player) const {
    if(player == State::PLAYER_O)
        return State::PLAYER_X;

    return State::PLAYER_O;
}

int Bot::evaluateBoard() const {
    switch(heuristic) {
        case HeuristicType::SUCCESS_OR_DEFEAT:
            return successOrDefeatHeuristic();
        default:
            return 0;
    }
}