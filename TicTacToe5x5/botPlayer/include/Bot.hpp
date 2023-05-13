#ifndef BOT_HPP
#define BOT_HPP

#include <iostream>
#include <random>
#include <algorithm>
#include <limits.h>

#include "Board.hpp"

enum class AlgorithmType {
    MIN_MAX,
    ALPHA_BETA_PRUNING
};

class Bot {
public:  
    struct MoveAndScore {
        unsigned short move;
        int score;
    };

    unsigned short depth;
    HeuristicType heuristic;
    bool starter;
    Board board;
    std::mt19937 mersenneTwister;

public:
    Bot(unsigned short maximalDepth, HeuristicType heuristicType, bool hasFirstMove);
    Bot(Board initialBoard, unsigned short maximalDepth, HeuristicType heuristicType, bool hasFirstMove);
    

    int opponentMove(unsigned short row, unsigned short column);
    int move(AlgorithmType algorithmType);
    int randomMove();

private:
    MoveAndScore minMax(unsigned short depth, bool isMax);
    MoveAndScore alphaBetaPruning(unsigned short depth, bool isMax, int alpha, int beta);
};

#endif // BOT_HPP