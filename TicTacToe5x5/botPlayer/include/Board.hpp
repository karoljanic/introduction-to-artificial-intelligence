#ifndef BOARD_HPP
#define BOARD_HPP

#include <ostream>
#include <iostream>
#include <vector>
#include <algorithm>
#include <limits.h>

enum class State {
    EMPTY,              // '_'
    BOT_MARKER,         // 'X'
    OPPONENT_MARKER     // 'O'
};

enum class HeuristicType {
    SUCCESS_OR_DEFEAT,
    MIN_MOVES_TO_WIN
};


class Board {
    static constexpr unsigned short boardSize = 5;

    static constexpr unsigned short winStates[28][4][2] = { 
        { {0,0}, {0,1}, {0,2}, {0,3} },
        { {1,0}, {1,1}, {1,2}, {1,3} },
        { {2,0}, {2,1}, {2,2}, {2,3} },
        { {3,0}, {3,1}, {3,2}, {3,3} },
        { {4,0}, {4,1}, {4,2}, {4,3} },
        { {0,1}, {0,2}, {0,3}, {0,4} },
        { {1,1}, {1,2}, {1,3}, {1,4} },
        { {2,1}, {2,2}, {2,3}, {2,4} },
        { {3,1}, {3,2}, {3,3}, {3,4} },
        { {4,1}, {4,2}, {4,3}, {4,4} },
        { {0,0}, {1,0}, {2,0}, {3,0} },
        { {0,1}, {1,1}, {2,1}, {3,1} },
        { {0,2}, {1,2}, {2,2}, {3,2} },
        { {0,3}, {1,3}, {2,3}, {3,3} },
        { {0,4}, {1,4}, {2,4}, {3,4} },
        { {1,0}, {2,0}, {3,0}, {4,0} },
        { {1,1}, {2,1}, {3,1}, {4,1} },
        { {1,2}, {2,2}, {3,2}, {4,2} },
        { {1,3}, {2,3}, {3,3}, {4,3} },
        { {1,4}, {2,4}, {3,4}, {4,4} },
        { {0,1}, {1,2}, {2,3}, {3,4} },
        { {0,0}, {1,1}, {2,2}, {3,3} },
        { {1,1}, {2,2}, {3,3}, {4,4} },
        { {1,0}, {2,1}, {3,2}, {4,3} },
        { {0,3}, {1,2}, {2,1}, {3,0} },
        { {0,4}, {1,3}, {2,2}, {3,1} },
        { {1,3}, {2,2}, {3,1}, {4,0} },
        { {1,4}, {2,3}, {3,2}, {4,1} }
    };

    static constexpr unsigned short loseStates[48][3][2] = {
        { {0,0}, {0,1}, {0,2} }, { {0,1}, {0,2}, {0,3} }, { {0,2}, {0,3}, {0,4} }, 
        { {1,0}, {1,1}, {1,2} }, { {1,1}, {1,2}, {1,3} }, { {1,2}, {1,3}, {1,4} }, 
        { {2,0}, {2,1}, {2,2} }, { {2,1}, {2,2}, {2,3} }, { {2,2}, {2,3}, {2,4} }, 
        { {3,0}, {3,1}, {3,2} }, { {3,1}, {3,2}, {3,3} }, { {3,2}, {3,3}, {3,4} }, 
        { {4,0}, {4,1}, {4,2} }, { {4,1}, {4,2}, {4,3} }, { {4,2}, {4,3}, {4,4} }, 
        { {0,0}, {1,0}, {2,0} }, { {1,0}, {2,0}, {3,0} }, { {2,0}, {3,0}, {4,0} }, 
        { {0,1}, {1,1}, {2,1} }, { {1,1}, {2,1}, {3,1} }, { {2,1}, {3,1}, {4,1} }, 
        { {0,2}, {1,2}, {2,2} }, { {1,2}, {2,2}, {3,2} }, { {2,2}, {3,2}, {4,2} }, 
        { {0,3}, {1,3}, {2,3} }, { {1,3}, {2,3}, {3,3} }, { {2,3}, {3,3}, {4,3} }, 
        { {0,4}, {1,4}, {2,4} }, { {1,4}, {2,4}, {3,4} }, { {2,4}, {3,4}, {4,4} }, 
        { {0,2}, {1,3}, {2,4} }, { {0,1}, {1,2}, {2,3} }, { {1,2}, {2,3}, {3,4} }, 
        { {0,0}, {1,1}, {2,2} }, { {1,1}, {2,2}, {3,3} }, { {2,2}, {3,3}, {4,4} }, 
        { {1,0}, {2,1}, {3,2} }, { {2,1}, {3,2}, {4,3} }, { {2,0}, {3,1}, {4,2} }, 
        { {0,2}, {1,1}, {2,0} }, { {0,3}, {1,2}, {2,1} }, { {1,2}, {2,1}, {3,0} }, 
        { {0,4}, {1,3}, {2,2} }, { {1,3}, {2,2}, {3,1} }, { {2,2}, {3,1}, {4,0} }, 
        { {1,4}, {2,3}, {3,2} }, { {2,3}, {3,2}, {4,1} }, { {2,4}, {3,3}, {4,2} }
    };

    State boardStates[boardSize][boardSize];
    unsigned short turnNumber;

public:
    Board();
    Board(std::string states);
    void makeMove(unsigned short row, unsigned short col, State player, int turnUpdate);
    bool isWinningState(State player) const;
    bool isLosingState(State player) const;
    bool isDrawState() const;
    bool isEndState() const;
    void display(std::ostream& stream) const;
    std::vector<int> getPossibleMoves() const;
    unsigned short getTurnNumber() const;

    int evaluate(HeuristicType heuristic) const;
    int successOrDefeatHeuristic() const;
    int minMovesToWinHeuristic() const;
};

#endif // BOARD_HPP