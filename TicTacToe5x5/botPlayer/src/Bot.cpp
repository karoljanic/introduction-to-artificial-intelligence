#include "Bot.hpp"

Bot::Bot(unsigned short maximalDepth, HeuristicType heuristicType, bool hasFirstMove): 
    depth{maximalDepth}, 
    heuristic{heuristicType},
    starter{hasFirstMove},
    //board{Board{HybridHeuristicsWeights{100, 100, -5, 70, 100, 100, 100, 100, 0}}},
    board{Board{HybridHeuristicsWeights{100, 511, 511, 511, 0, 0, 1, 1, 1}}},
    mersenneTwister{time(0)} { }

Bot::Bot(unsigned short maximalDepth, HeuristicType heuristicType, HybridHeuristicsWeights heuristicsWeights, bool hasFirstMove):
    depth{maximalDepth}, 
    heuristic{heuristicType},
    starter{hasFirstMove},
    board{Board{heuristicsWeights}},
    mersenneTwister{time(0)} { }

Bot::Bot(Board initialBoard, unsigned short maximalDepth, HeuristicType heuristicType, bool hasFirstMove):
    depth{maximalDepth}, 
    heuristic{heuristicType},
    starter{hasFirstMove},
    board{initialBoard},
    mersenneTwister{time(0)} { }

void Bot::opponentMove(unsigned short row, unsigned short column) {
    board.makeMove(row, column, State::OPPONENT_MARKER, 1);

    //board.display(std::cout);
}

int Bot::move(AlgorithmType algorithmType) {
    MoveAndScore moveAndScore;

    /*
    if(board.getTurnNumber() == 0) {
        moveAndScore.move = 33;
    }
    else if(board.getTurnNumber() == 1) {
        if(board.isEmpty(3, 3)) {
            moveAndScore.move = 33;
        }
        else {
            std::uniform_int_distribution<unsigned short> distribution{0, 3};
            unsigned short moveId = distribution(mersenneTwister);

            if(moveId == 0)
                moveAndScore.move = 23;
            else if(moveId == 1)
                moveAndScore.move = 43;
            else if(moveId == 2)
                moveAndScore.move = 32;
            else
                moveAndScore.move = 34;
        }
    }
    else {
        */
        if(algorithmType == AlgorithmType::MIN_MAX) 
            moveAndScore = minMax(depth, true);
        else
            moveAndScore = alphaBetaPruning(depth, true, INT_MIN, INT_MAX);
    //}

    board.makeMove(moveAndScore.move / 10, moveAndScore.move % 10, State::BOT_MARKER, 1);
    //board.display(std::cout);

    return moveAndScore.move;
}

int Bot::randomMove() {
    std::vector<int> possibleMoves{board.getPossibleMoves()};
    std::uniform_int_distribution<unsigned short> distribution{0, possibleMoves.size() - 1};

    int move = possibleMoves[distribution(mersenneTwister)];

    board.makeMove(move / 10, move % 10, State::BOT_MARKER, 1);
    board.display(std::cout);

    return move;
}

int Bot::gameState() {
    if(board.isWinningState(State::BOT_MARKER) || board.isLosingState(State::OPPONENT_MARKER))
        return 2;
    
    if(board.isWinningState(State::OPPONENT_MARKER) || board.isLosingState(State::BOT_MARKER))
        return 1;

    if(board.isDrawState())
        return 0;

    return -1;
}

Bot::MoveAndScore Bot::minMax(unsigned short depth, bool isMax) {
    if(board.isWinningState(State::BOT_MARKER) || board.isLosingState(State::OPPONENT_MARKER))
        return {0, INT_MAX / 2};

    if(board.isWinningState(State::OPPONENT_MARKER) || board.isLosingState(State::BOT_MARKER))
        return {0, INT_MIN / 2};

    if(board.isDrawState())
        return {0, 0};

    if(depth == 0) {
        return {0, board.evaluate(heuristic)};
    }

    std::vector<MoveAndScore> moveScores;
    for(const int& move: board.getPossibleMoves()) {
        unsigned short x = move / 10;
        unsigned short y = move % 10;

        if(isMax) {
            board.makeMove(x, y, State::BOT_MARKER, 1);
            MoveAndScore moveAndScore = minMax(depth - 1, !isMax);
            moveAndScore.move = move;
            moveScores.push_back(moveAndScore);
            board.makeMove(x, y, State::EMPTY, -1);
        }
        else {
            board.makeMove(x, y, State::OPPONENT_MARKER, 1);
            MoveAndScore moveAndScore = minMax(depth - 1, !isMax);
            moveAndScore.move = move;
            moveScores.push_back(moveAndScore);
            board.makeMove(x, y, State::EMPTY, -1);
        }
    }

    if(isMax) {
        std::sort(moveScores.begin(), moveScores.end(), 
        [](const MoveAndScore& a, const MoveAndScore& b) -> bool { return a.score > b.score; });
    }
    else {
        std::sort(moveScores.begin(), moveScores.end(), 
        [](const MoveAndScore& a, const MoveAndScore& b) -> bool { return a.score < b.score; });
    }

    unsigned int equalScoresCounter = 1;
    while(equalScoresCounter < moveScores.size() && moveScores[equalScoresCounter].score == moveScores[0].score)
        equalScoresCounter++;

    std::uniform_int_distribution<unsigned short> distribution{0, equalScoresCounter - 1};
    unsigned short moveIndex = distribution(mersenneTwister);

    return moveScores[moveIndex];
}

Bot::MoveAndScore Bot::alphaBetaPruning(unsigned short depth, bool isMax, int alpha, int beta) {
    if(board.isWinningState(State::BOT_MARKER) || board.isLosingState(State::OPPONENT_MARKER))
        return {0, INT_MAX / 2};

    if(board.isWinningState(State::OPPONENT_MARKER) || board.isLosingState(State::BOT_MARKER))
        return {0, INT_MIN / 2};

    if(board.isDrawState())
        return {0, 0};

    if(depth == 0) {
        return {0, board.evaluate(heuristic)};
    }

    MoveAndScore bestResult;
    for(const int& move: board.getPossibleMoves()) {
        unsigned short x = move / 10;
        unsigned short y = move % 10;

        if(isMax) {
            board.makeMove(x, y, State::BOT_MARKER, 1);
            MoveAndScore moveAndScore = alphaBetaPruning(depth - 1, !isMax, alpha, beta);
            board.makeMove(x, y, State::EMPTY, -1);

            if(moveAndScore.score > alpha) {
                moveAndScore.move = move;
                alpha = moveAndScore.score;
                bestResult = moveAndScore;
            }
        }
        else {
            board.makeMove(x, y, State::OPPONENT_MARKER, 1);
            MoveAndScore moveAndScore = alphaBetaPruning(depth - 1, !isMax, alpha, beta);
            board.makeMove(x, y, State::EMPTY, -1);

            if(moveAndScore.score < beta) {
                moveAndScore.move = move;
                beta = moveAndScore.score;
                bestResult = moveAndScore;
            }
        }  

        if(alpha > beta)
            break;
    }

    return bestResult;
}
