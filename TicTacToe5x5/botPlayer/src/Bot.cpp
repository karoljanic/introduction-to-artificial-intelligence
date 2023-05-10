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
    board.makeMove(row, column, opponentRole);
    board.nextTurn();

    board.display(std::cout);
    return 0;
}

int Bot::move() {
    unsigned short move;
    minMax(botRole, depth, &move);

    board.makeMove(move / 10, move % 10, botRole);
    board.nextTurn();
    board.display(std::cout);

    return move;
}

int Bot::randomMove() {
    std::vector<int> possibleMoves{board.getPossibleMoves()};
    std::uniform_int_distribution<unsigned short> distribution{0, possibleMoves.size() - 1};

    int move = possibleMoves[distribution(mersenneTwister)];

    board.makeMove(move / 10, move % 10, botRole);
    board.nextTurn();
    board.display(std::cout);

    return move;
}

int Bot::minMax(State player, unsigned short depth, unsigned short* bestMove) {
    if(depth == 0 || board.isEndState()) {
        return evaluateBoard(player);
    }

    int bestScore;
    if(player == botRole)
        bestScore = -INF;
    else
        bestScore = INF;

    for(const int& move: board.getPossibleMoves()) {
        unsigned short x = move / 10;
        unsigned short y = move % 10;

        board.makeMove(x, y, player);
        int score = minMax(getOppositePlayer(player), depth - 1, bestMove);
        board.makeMove(x, y, State::EMPTY);

        if(player == botRole) {
            if(score > bestScore) {
                bestScore = score;
                *bestMove = move;
            }
        }
        else {
            if(score < bestScore) {
                bestScore = score;
                *bestMove = move;
            }
        }
    }

    return bestScore;
}

int Bot::alphaBetaPruning(State player, unsigned short depth, unsigned short* bestMove) {
    
}

State Bot::getOppositePlayer(State player) const {
    if(player == State::PLAYER_O)
        return State::PLAYER_X;

    return State::PLAYER_O;
}

int Bot::evaluateBoard(State player) const {
    switch(heuristic) {
        case HeuristicType::SUCCESS_OR_DEFEAT:
            return successOrDefeatHeuristic(player);
        default:
            return 0;
    }
}

int Bot::successOrDefeatHeuristic(State player) const {
    if(board.isWinningState(player) || board.isLosingState(getOppositePlayer(player)))
        return 1;

    if(board.isLosingState(player) || board.isWinningState(getOppositePlayer(player)))
        return -1;

    return 0;
}

