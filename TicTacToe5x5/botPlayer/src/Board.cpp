#include "Board.hpp"

Board::Board(): 
    turnNumber{0} {

    for(unsigned short row = 0; row < boardSize; row++) {
        for(unsigned short col = 0; col < boardSize; col++) {
            boardStates[row][col] = State::EMPTY;
        }
    }
}

Board::Board(std::string states): turnNumber{0} {
    if(states.size() != boardSize * boardSize) {
        for(unsigned short row = 0; row < boardSize; row++) {
            for(unsigned short col = 0; col < boardSize; col++) {
                boardStates[row][col] = State::EMPTY;
            }
        }
    }
    else {
        for(unsigned short row = 0; row < boardSize; row++) {
            for(unsigned short col = 0; col < boardSize; col++) {
                if(states[row * boardSize + col] == 'X' || states[row * boardSize + col] == 'x')
                    boardStates[row][col] = State::BOT_MARKER;
                else if(states[row * boardSize + col] == 'O' || states[row * boardSize + col] == 'o')
                    boardStates[row][col] = State::OPPONENT_MARKER;
                else
                    boardStates[row][col] = State::EMPTY;
            }
        }
    }
}

void Board::makeMove(unsigned short row, unsigned short col, State player, int turnUpdate) {
    boardStates[row - 1][col - 1] = player;
    turnNumber += turnUpdate;
}

bool Board::isWinningState(State player) const {
    bool win = false;
    for(int i = 0; i < 28; i++) {
        if(boardStates[winStates[i][0][0]][winStates[i][0][1]] == player && 
           boardStates[winStates[i][1][0]][winStates[i][1][1]] == player && 
           boardStates[winStates[i][2][0]][winStates[i][2][1]] == player && 
           boardStates[winStates[i][3][0]][winStates[i][3][1]] == player) { win = true; }
    }

    return win;
}

bool Board::isLosingState(State player) const {
    if(isWinningState(player))
        return false;
        
    bool loose = false;
    for(int i = 0; i < 48; i++)
    if(boardStates[loseStates[i][0][0]][loseStates[i][0][1]] == player && 
       boardStates[loseStates[i][1][0]][loseStates[i][1][1]] == player && 
       boardStates[loseStates[i][2][0]][loseStates[i][2][1]] == player) { loose = true; }

    return loose;
}

bool Board::isDrawState() const {
    return turnNumber == boardSize * boardSize;
}

bool Board::isEndState() const {
    return isWinningState(State::BOT_MARKER) || isWinningState(State::OPPONENT_MARKER) || 
           isLosingState(State::BOT_MARKER) || isLosingState(State::OPPONENT_MARKER) || 
           isDrawState();
}

void Board::display(std::ostream& stream) const {
    stream << "  ";
    for(unsigned short col = 1; col <= boardSize; col++)
        stream << col << ' ';
    stream << '\n';

    for(unsigned short row = 0; row < boardSize; row++) {
        stream << row + 1;
        for(unsigned short col = 0; col < boardSize; col++) {
            switch(boardStates[row][col]) {
                case State::EMPTY: stream << " -"; break;
                case State::BOT_MARKER: stream << " X"; break;
                case State::OPPONENT_MARKER: stream << " O"; break;
            }
        }
        stream << '\n';
    }
}

std::vector<int> Board::getPossibleMoves() const {
    std::vector<int> possibleMoves{};
    
    for(unsigned short row = 0; row < boardSize; row++) {
        for(unsigned short col = 0; col < boardSize; col++) {
            if(boardStates[row][col] == State::EMPTY)
                possibleMoves.emplace_back(10 * (row + 1) + col + 1);
        }
    }

    return possibleMoves;
}

unsigned short Board::getTurnNumber() const {
    return turnNumber;
}

int Board::evaluate(HeuristicType heuristic) const {
    switch(heuristic) {
        case HeuristicType::SUCCESS_OR_DEFEAT:
            return successOrDefeatHeuristic();
        case HeuristicType::MIN_MOVES_TO_WIN:
            return minMovesToWinHeuristic();
        default:
            return 0;
    }
}

int Board::successOrDefeatHeuristic() const {
    if(isWinningState(State::BOT_MARKER) || isLosingState(State::OPPONENT_MARKER))
        return INT_MAX;

    if(isLosingState(State::BOT_MARKER) || isWinningState(State::OPPONENT_MARKER))
        return INT_MIN;

    return 0;
}

int Board::minMovesToWinHeuristic() const {
    if(isWinningState(State::BOT_MARKER) || isLosingState(State::OPPONENT_MARKER))
        return 10000;
    
    if(isWinningState(State::OPPONENT_MARKER) || isLosingState(State::BOT_MARKER))
        return 10000;

    if(isDrawState())
        return 0;

    std::vector<int> minMovesToBotWin;
    std::vector<int> minMovesToOpponentWin;

    for(int i = 0; i < 28; i++) {
        int minMoves = 0;
        if(boardStates[winStates[i][0][0]][winStates[i][0][1]] != State::BOT_MARKER)
            minMoves++;
        if(boardStates[winStates[i][1][0]][winStates[i][1][1]] != State::BOT_MARKER)
            minMoves++;
        if(boardStates[winStates[i][2][0]][winStates[i][2][1]] != State::BOT_MARKER)
            minMoves++;
        if(boardStates[winStates[i][3][0]][winStates[i][3][1]] != State::BOT_MARKER)
            minMoves++;

        minMovesToBotWin.push_back(minMoves);

        minMoves = 0;
        if(boardStates[winStates[i][0][0]][winStates[i][0][1]] != State::OPPONENT_MARKER)
            minMoves++;
        if(boardStates[winStates[i][1][0]][winStates[i][1][1]] != State::OPPONENT_MARKER)
            minMoves++;
        if(boardStates[winStates[i][2][0]][winStates[i][2][1]] != State::OPPONENT_MARKER)
            minMoves++;
        if(boardStates[winStates[i][3][0]][winStates[i][3][1]] != State::OPPONENT_MARKER)
            minMoves++;

        minMovesToOpponentWin.push_back(minMoves);
    }

    std::sort(minMovesToBotWin.begin(), minMovesToBotWin.end());
    int minValBot = minMovesToBotWin.front();
    int minCountBot = 0;
    for(const int& count: minMovesToBotWin) {
        if(count == minValBot)
            minCountBot++;
    }

    std::sort(minMovesToOpponentWin.begin(), minMovesToOpponentWin.end());
    int minValOpponent = minMovesToOpponentWin.front();
    int minCountOpponent = 0;
    for(const int& count: minMovesToOpponentWin) {
        if(count == minValOpponent)
            minCountOpponent++;
    }

    //return (4 - minValBot) * (2 << minCountBot) - (4 - minValOpponent) * (2 << minCountOpponent);
    return minValOpponent - minValBot;
    //return 4 - minValBot;
}
