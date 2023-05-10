#include "Board.hpp"

Board::Board(): 
    turnNumber{0} {

    for(unsigned short row = 0; row < boardSize; row++) {
        for(unsigned short col = 0; col < boardSize; col++) {
            boardStates[row][col] = State::EMPTY;
        }
    }
}

void Board::nextMove(unsigned short row, unsigned short col, State player) {
    turnNumber++;
    boardStates[row - 1][col - 1] = player;
}

bool Board::isWinningState(State player) const {
    bool win = false;
    for(int i = 0; i < 28; i++) {
        if(boardStates[winStates[i][0][0]][winStates[i][0][1]] == player && 
           boardStates[winStates[i][1][0]][winStates[i][1][1]] == player && 
           boardStates[winStates[i][2][0]][winStates[i][2][1]] == player && 
           boardStates[winStates[i][3][0]][winStates[i][3][1]] == player) { win=true; }
    }

    return win;
}

bool Board::isLosingState(State player) const {
    bool loose = false;
    for(int i = 0; i < 48; i++)
    if(boardStates[loseStates[i][0][0]][loseStates[i][0][1]] == player && 
       boardStates[loseStates[i][1][0]][loseStates[i][1][1]] == player && 
       boardStates[loseStates[i][2][0]][loseStates[i][2][1]] == player) { loose = true; }

    return loose;
}

bool Board::isEndState() const {
    return isWinningState(State::PLAYER_O) || isWinningState(State::PLAYER_X) || 
           isLosingState(State::PLAYER_O) || isLosingState(State::PLAYER_X) || 
           turnNumber == boardSize * boardSize;
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
                case State::EMPTY: printf(" -"); break;
                case State::PLAYER_X: printf(" X"); break;
                case State::PLAYER_O: printf(" O"); break;
            }
        }
        stream << '\n';
    }
    stream << '\n';
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