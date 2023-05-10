#ifndef BOT_HPP
#define BOT_HPP

#include <iostream>
#include <random>

#include "Board.hpp"

enum class HeuristicType {
    SUCCESS_OR_DEFEAT
};

class Bot {
    unsigned short depth;
    HeuristicType heuristic;
    bool starter;
    Board board;
    State botRole;
    State opponentRole;
    std::mt19937 mersenneTwister;

public:
    Bot(unsigned short maximalDepth, HeuristicType heuristicType, bool hasFirstMove);

    int opponentMove(unsigned short row, unsigned short column);
    int move();
    int randomMove();

private:
    int minMax(State player, unsigned short depth, unsigned short* bestX, unsigned short* bestY);

    State getOppositePlayer(State player) const;
    int evaluateBoard() const;
    int successOrDefeatHeuristic() const;
};


#endif // BOT_HPP