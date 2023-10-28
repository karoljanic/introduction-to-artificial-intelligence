#include "Board.hpp"

Board::Board(): 
    turnNumber{0},
    hybridHeuristicsWeights{1, 1, 1, 1, 1, 1, 1, 1, 1} {

    for(unsigned short row = 0; row < boardSize; row++) {
        for(unsigned short col = 0; col < boardSize; col++) {
            boardStates[row][col] = State::EMPTY;
        }
    }
}

Board::Board(std::string states): 
    turnNumber{0},
    hybridHeuristicsWeights{1, 1, 1, 1, 1, 1, 1, 1, 1} {
        
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

Board::Board(State (&states)[5][5], HybridHeuristicsWeights heuristicsWeights):
    turnNumber{0},
    hybridHeuristicsWeights{heuristicsWeights} {
    
    for(unsigned short row = 0; row < boardSize; row++) {
        for(unsigned short col = 0; col < boardSize; col++) {
            boardStates[row][col] = states[row][col];
        }
    }
}

Board::Board(HybridHeuristicsWeights heuristicsWeights):
    turnNumber{0},
    hybridHeuristicsWeights{heuristicsWeights} { 

    for(unsigned short row = 0; row < boardSize; row++) {
        for(unsigned short col = 0; col < boardSize; col++) {
            boardStates[row][col] = State::EMPTY;
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
    return getPossibleMoves().empty();
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

 bool Board::isEmpty(unsigned int row, unsigned short col) const {
    return boardStates[row - 1][col - 1] == State::EMPTY;
 }

int Board::evaluate(HeuristicType heuristic) {
    switch(heuristic) {
        case HeuristicType::SUCCESS_OR_DEFEAT:
            return successOrDefeatHeuristic();
        case HeuristicType::MIN_MOVES_TO_WIN:
            return minMovesToWinHeuristic();
        case HeuristicType::HYBRID:
            return hybridHeuristic();
        default:
            return 0;
    }
}

int Board::successOrDefeatHeuristic() {
    if(isWinningState(State::BOT_MARKER) || isLosingState(State::OPPONENT_MARKER))
        return INT_MAX / 2;

    if(isLosingState(State::BOT_MARKER) || isWinningState(State::OPPONENT_MARKER))
        return INT_MIN / 2;

    return 0;
}

int Board::minMovesToWinHeuristic() {
    if(isWinningState(State::BOT_MARKER) || isLosingState(State::OPPONENT_MARKER))
        return INT_MAX / 2;
    
    if(isWinningState(State::OPPONENT_MARKER) || isLosingState(State::BOT_MARKER))
        return INT_MIN / 2;

    if(isDrawState())
        return 0;

    std::vector<int> minMovesToBotWin = movesToWin(State::BOT_MARKER);
    std::vector<int> minMovesToOpponentWin = movesToWin(State::OPPONENT_MARKER);
    
    /*
    for(int i = 0; i < 28; i++) {
        int emptyStates = 0;
        int botMarkers = 0;
        int opponentMakers = 0;

        if(boardStates[winStates[i][0][0]][winStates[i][0][1]] == State::BOT_MARKER)
            botMarkers++;
        else if(boardStates[winStates[i][0][0]][winStates[i][0][1]] == State::OPPONENT_MARKER)
            opponentMakers++;
        else
            emptyStates++;

        if(boardStates[winStates[i][1][0]][winStates[i][1][1]] == State::BOT_MARKER)
            botMarkers++;
        else if(boardStates[winStates[i][1][0]][winStates[i][1][1]] == State::OPPONENT_MARKER)
            opponentMakers++;
        else
            emptyStates++;

        if(boardStates[winStates[i][2][0]][winStates[i][2][1]] == State::BOT_MARKER)
            botMarkers++;
        else if(boardStates[winStates[i][2][0]][winStates[i][2][1]] == State::OPPONENT_MARKER)
            opponentMakers++;
        else
            emptyStates++;

        if(boardStates[winStates[i][3][0]][winStates[i][3][1]] == State::BOT_MARKER)
            botMarkers++;
        else if(boardStates[winStates[i][3][0]][winStates[i][3][1]] == State::OPPONENT_MARKER)
            opponentMakers++;
        else
            emptyStates++;

        if(botMarkers == opponentMakers) {
            minMovesToBotWin.push_back(emptyStates);
            minMovesToOpponentWin.push_back(emptyStates);
        }
        else if(botMarkers > opponentMakers) {
            minMovesToBotWin.push_back(emptyStates);
        }
        else {
            minMovesToOpponentWin.push_back(emptyStates);
        }
    }
    */

    int minValBot = 4;
    int minCountBot = 0;
    if(minMovesToBotWin.size() > 0) {
        std::sort(minMovesToBotWin.begin(), minMovesToBotWin.end());
        minValBot = minMovesToBotWin.front();
        minCountBot = 0;
        for(const int& count: minMovesToBotWin) {
            if(count == minValBot)
                minCountBot++;
        }
    }

    int minValOpponent = 4;
    int minCountOpponent = 0;

    if(minMovesToOpponentWin.size() > 0) {
        std::sort(minMovesToOpponentWin.begin(), minMovesToOpponentWin.end());
        minValOpponent = minMovesToOpponentWin.front();
        minCountOpponent = 0;
        for(const int& count: minMovesToOpponentWin) {
            if(count == minValOpponent)
                minCountOpponent++;
        }
    }

    return minValOpponent - minValBot;

    /*
    int minValBotReversed = 4 - minValBot;
    if(minValBotReversed == 1)
        minValBotReversed *= 1;
    else if(minValBotReversed == 2)
        minValBotReversed *= 3;
    else if(minValBotReversed == 3)
        minValBotReversed *= 7;

    int minValOpponentReversed = 4 - minValOpponent;
    if(minValOpponentReversed == 1)
        minValOpponentReversed *= 1;
    else if(minValOpponentReversed == 2)
        minValOpponentReversed *= 3;
    else if(minValOpponentReversed == 3)
        minValOpponentReversed *= 7;

    return minValBotReversed * minCountBot - minValOpponentReversed * minCountOpponent;
    */
}

std::vector<int> Board::movesToWin(State player) {
    State opponent;
    if(player == State::BOT_MARKER)
        opponent = State::OPPONENT_MARKER;
    else    
        opponent = State::BOT_MARKER;

    std::vector<int> result;

    int tmp;
    int empty;

    // checking patterns on rows
    for(unsigned short row = 0; row < boardSize; row++) {
        tmp = canBeRowWinning(row, player, opponent);
        if(tmp == -1)
            continue;

        empty = 0;
        for(unsigned short col = tmp; col < (boardSize + tmp - 1); col++) {
            if(boardStates[row][col] == State::EMPTY)
                empty++;
        }
        result.push_back(empty);
    }

    // checking patterns on columns
    for(unsigned short col = 0; col < boardSize; col++) {
        tmp = canBeColWinning(col, player, opponent);
        if(tmp == -1)
            continue;

        empty = 0;
        for(unsigned short row = tmp; row < (boardSize + tmp - 1); row++) {
            if(boardStates[row][col] == State::EMPTY)
                empty++;
        }
        result.push_back(empty);
    }

    // checking patterns on diagonals of length 5
    State diagonalsLength5[2][5] = {
        { boardStates[0][0], boardStates[1][1], boardStates[2][2], boardStates[3][3], boardStates[4][4] },
        { boardStates[0][4], boardStates[1][3], boardStates[2][2], boardStates[3][1], boardStates[4][0] }
    };

    for(unsigned short diag = 0; diag < 2; diag++) {
        tmp = canBeDiag5Winning(diagonalsLength5[diag], player, opponent);
        if(tmp == -1)
            continue;

        empty = 0;
        for(unsigned short idx = tmp; idx < (boardSize - 1 + tmp); idx++) {
            if(diagonalsLength5[diag][idx] == State::EMPTY)
                empty++;
        }
        result.push_back(empty);
    }

    // checking patterns on diagonals of length 4
    State diagonalsLength4[4][4] = {
        { boardStates[0][1], boardStates[1][2], boardStates[2][3], boardStates[3][4] },
        { boardStates[1][0], boardStates[2][1], boardStates[3][2], boardStates[4][3] },
        { boardStates[0][3], boardStates[1][2], boardStates[2][1], boardStates[3][0] },
        { boardStates[1][4], boardStates[2][3], boardStates[3][2], boardStates[4][1] }
    };

    for(unsigned short diag = 0; diag < 4; diag++) {
        tmp = canBeDiag4Winning(diagonalsLength4[diag], player, opponent);
        if(tmp == -1)
            continue;

        empty = 0;
        for(unsigned short idx = 0; idx < (boardSize - 1); idx++) {
            if(diagonalsLength4[diag][idx] == State::EMPTY)
                empty++;
        }
        result.push_back(empty);
    }

    return result;
}

int Board::canBeRowWinning(unsigned short row, State bot, State opponent) {
    unsigned short opponents0 = 0;
    unsigned short bots0 = 0;
    for(unsigned short col = 0; col < (boardSize - 1); col++) {
        if(boardStates[row][col] == bot)
            bots0++;
        else if(boardStates[row][col] == opponent)
            opponents0++;
    }

    unsigned short opponents1 = 0;
    unsigned short bots1 = 0;
    for(unsigned short col = 1; col < boardSize; col++) {
        if(boardStates[row][col] == bot)
            bots1++;
        else if(boardStates[row][col] == opponent)
            opponents1++;
    }
    
    if(opponents0 > 0 && opponents1 > 0) {
        return -1;
    }
    else if(opponents0 > 0) {
        return 1;
    }
    else if(opponents1 > 0) {
        return 0;
    }
    else { // opponents0 == 0 && opponents1 == 0
        if(bots0 > bots1)
            return 0;
        else    
            return 1;
    }
}

int Board::canBeColWinning(unsigned short col, State bot, State opponent) {
    unsigned short opponents0 = 0;
    unsigned short bots0 = 0;
    for(unsigned short row = 0; row < (boardSize - 1); row++) {
        if(boardStates[row][col] == bot)
            bots0++;
        else if(boardStates[row][col] == opponent)
            opponents0++;
    }

    unsigned short opponents1 = 0;
    unsigned short bots1 = 0;
    for(unsigned short row = 1; row < boardSize; row++) {
        if(boardStates[row][col] == bot)
            bots1++;
        else if(boardStates[row][col] == opponent)
            opponents1++;
    }

    if(opponents0 > 0 && opponents1 > 0) {
        return -1;
    }
    else if(opponents0 > 0) {
        return 1;
    }
    else if(opponents1 > 0) {
        return 0;
    }
    else { // opponents0 == 0 && opponents1 == 0
        if(bots0 > bots1)
            return 0;
        else    
            return 1;
    }
}

int Board::canBeDiag5Winning(State* diagonal, State bot, State opponent) {
    unsigned short opponents0 = 0;
    unsigned short bots0 = 0;
    for(unsigned short idx = 0; idx < (boardSize - 1); idx++) {
        if(diagonal[idx] == bot)
            bots0++;
        else if(diagonal[idx] == opponent)
            opponents0++;
    }

    unsigned short opponents1 = 0;
    unsigned short bots1 = 0;
    for(unsigned short idx = 1; idx < boardSize; idx++) {
        if(diagonal[idx] == bot)
            bots1++;
        else if(diagonal[idx] == opponent)
            opponents1++;
    }

    if(opponents0 > 0 && opponents1 > 0) {
        return -1;
    }
    else if(opponents0 > 0) {
        return 1;
    }
    else if(opponents1 > 0) {
        return 0;
    }
    else { // opponents0 == 0 && opponents1 == 0
        if(bots0 > bots1)
            return 0;
        else    
            return 1;
    }
}

int Board::canBeDiag4Winning(State* diagonal, State bot, State opponent) {
    if(diagonal[0] != opponent &&
       diagonal[1] != opponent &&
       diagonal[2] != opponent &&
       diagonal[3] != opponent) { return 0; }

    return -1;
}

int Board::hybridHeuristic() {
    if(isWinningState(State::BOT_MARKER) || isLosingState(State::OPPONENT_MARKER))
        return INT_MAX / 2;
    
    if(isWinningState(State::OPPONENT_MARKER) || isLosingState(State::BOT_MARKER))
        return INT_MIN / 2;

    if(isDrawState())
        return 0;

    HybridHeuristicComponents hybridHeuristicsComponents = calculateHybridHeuristicComponents();

    int heuristicValue = 0;
    heuristicValue += hybridHeuristicsWeights.movementFreedom * hybridHeuristicsComponents.movementFreedom;
    heuristicValue += hybridHeuristicsWeights.possibleWins * hybridHeuristicsComponents.possibleWins;
    heuristicValue += hybridHeuristicsWeights.possibleOpponentLoses * hybridHeuristicsComponents.possibleOpponentLoses;
    heuristicValue += hybridHeuristicsComponents.winLosePositions * hybridHeuristicsComponents.winLosePositions;
    heuristicValue += hybridHeuristicsWeights.doubleMarkerZeroEmptyBetween * hybridHeuristicsComponents.doubleMarkerZeroEmptyBetween;
    heuristicValue += hybridHeuristicsWeights.doubleMarkerOneEmptyBetween * hybridHeuristicsComponents.doubleMarkerOneEmptyBetween;
    heuristicValue += hybridHeuristicsWeights.doubleMarkerTwoEmptyBetween * hybridHeuristicsComponents.doubleMarkerTwoEmptyBetween;
    heuristicValue += hybridHeuristicsWeights.tripleMarkerOneEmptyBetween * hybridHeuristicsComponents.tripleMarkerOneEmptyBetween;
    heuristicValue += hybridHeuristicsWeights.tripleMarkerTwoEmptyBetween * hybridHeuristicsComponents.tripleMarkerTwoEmptyBetween;

    return heuristicValue;
}

HybridHeuristicComponents Board::calculateHybridHeuristicComponents() {
    HybridHeuristicComponents hybridHeuristicsComponentsBot = calculateHybridHeuristicComponentsForPlayer(State::BOT_MARKER);
    HybridHeuristicComponents hybridHeuristicsComponentsOpponent = calculateHybridHeuristicComponentsForPlayer(State::OPPONENT_MARKER);

    return {
        hybridHeuristicsComponentsBot.movementFreedom - hybridHeuristicsComponentsOpponent.movementFreedom,
        hybridHeuristicsComponentsBot.winLosePositions - hybridHeuristicsComponentsOpponent.winLosePositions,
        hybridHeuristicsComponentsBot.possibleWins - hybridHeuristicsComponentsOpponent.possibleWins,
        hybridHeuristicsComponentsBot.possibleOpponentLoses - hybridHeuristicsComponentsOpponent.possibleOpponentLoses,
        hybridHeuristicsComponentsBot.doubleMarkerZeroEmptyBetween - hybridHeuristicsComponentsOpponent.doubleMarkerZeroEmptyBetween,
        hybridHeuristicsComponentsBot.doubleMarkerOneEmptyBetween - hybridHeuristicsComponentsOpponent.doubleMarkerOneEmptyBetween,
        hybridHeuristicsComponentsBot.doubleMarkerTwoEmptyBetween - hybridHeuristicsComponentsOpponent.doubleMarkerTwoEmptyBetween,
        hybridHeuristicsComponentsBot.tripleMarkerOneEmptyBetween - hybridHeuristicsComponentsOpponent.tripleMarkerOneEmptyBetween,
        hybridHeuristicsComponentsBot.tripleMarkerTwoEmptyBetween - hybridHeuristicsComponentsOpponent.tripleMarkerTwoEmptyBetween,
    };
}

HybridHeuristicComponents Board::calculateHybridHeuristicComponentsForPlayer(State player) {
    /*
    * each component of the heuristics is calculated as the score of a given property for the bot 
    * minus the score of a given property for the opponent
    *
    * 
    * -> movementFreedom = number of movements no causing defeat
    * 
    * -> winLosePositions = number of movements blocking winning bot movement causing self-defeat
    * 
    * -> doubleMarkerZeroEmptyBetween = counting __XX or XX__ patterns 
    *    (no causing defeat like __XXX and not included in the more general patterns)
    *    (diagonals of lengths 4 and 5 are also considered)
    * 
    * -> doubleMarkerOneEmptyBetween = counting X_X_ or _X_X patterns
    *    (not included in the more general patterns)
    *    (diagonals of lengths 4 and 5 are also considered)
    * 
    * -> doubleMarkerTwoEmptyBetween = counting X__X patterns
    *    (not included in the more general patterns)
    *    (diagonals of lengths 4 and 5 are also considered)
    * 
    * -> tripleMarkerOneEmptyBetween = counting X_XX or XX_X or XX_XX patterns
    *    (not included in winning/losing patterns)
    *    (diagonals of lengths 4 and 5 are also considered)
    * 
    * -> tripleMarkerTwoEmptyBetween = counting X__XX or XX__X patterns
    *    (not containing winning/losing patterns)
    *    (diagonals of lengths 4 and 5 are also considered)
    * 
    * 
    * assumptions: game state is not winning and is not losing
    *            : the examples are described from the perspective of 'X'
    *            : '*' is equal to '_' or 'O' or 'X'
    */

    State opponent;
    if(player == State::BOT_MARKER)
        opponent = State::OPPONENT_MARKER;
    else
        opponent = State::BOT_MARKER;


    int movementFreedom = 0;
    int possibleWins = 0;
    int possibleOpponentLoses = 0;
    int winLosePositions = 0;
    int doubleMarkerZeroEmptyBetween = 0;
    int doubleMarkerOneEmptyBetween = 0;
    int doubleMarkerTwoEmptyBetween = 0;
    int tripleMarkerOneEmptyBetween = 0;
    int tripleMarkerTwoEmptyBetween = 0;

    /*
    // calculating movementFreedom
    for(unsigned short row = 0; row < boardSize; row++) {
        for(unsigned short col = 0; col < boardSize; col++) {
            if(boardStates[row][col] == State::EMPTY) {
                boardStates[row][col] = player;
                
                if(!isLosingState(player))
                    movementFreedom++;

                boardStates[row][col] = State::EMPTY;
            }
        }
    }

    // calculating possibleWins, possibleOpponentLoses, winLosePositions
    for(unsigned short row = 0; row < boardSize; row++) {
        for(unsigned short col = 0; col < boardSize; col++) {
            bool botWinState;
            bool opponentLoseState;
            if(boardStates[row][col] == State::EMPTY) {
                boardStates[row][col] = player;
                botWinState = isWinningState(player);

                boardStates[row][col] = opponent;
                opponentLoseState = isLosingState(opponent);

                boardStates[row][col] = State::EMPTY;

                if(botWinState && opponentLoseState)
                    winLosePositions++;
                else if(botWinState)
                    possibleWins++;
                else if(opponentLoseState)
                    possibleOpponentLoses++;
            }
        }
    }
    */
    

    // counting patterns on rows
    for(unsigned short row = 0; row < boardSize; row++) {
        // 1) XXXXX or XXXX* or *XXXX  ==  winning states don't appear here
        // 2) XXX** or *XXX* or **XXX  ==  losing states don't appear here

        // 3) XX_XX  ==  tripleMarkerOneEmptyBetween
        if(check5InRow(row, player, player, State::EMPTY, player, player)) {
            tripleMarkerOneEmptyBetween++;
            continue;
        }

        // 4) XX__X  ==  tripleMarkerTwoEmptyBetween
        if(check5InRow(row, player, player, State::EMPTY, State::EMPTY, player)) {
            tripleMarkerTwoEmptyBetween++;
            continue;
        }

        // 5) X__XX  ==  tripleMarkerTwoEmptyBetween
        if(check5InRow(row, player, State::EMPTY, State::EMPTY, player, player)) {
            tripleMarkerTwoEmptyBetween++;
            continue;
        }

        // 6) X_XX*  ==  tripleMarkerOneEmptyBetween
        // 7) *X_XX  ==  tripleMarkerOneEmptyBetween
        if(check4InRow(row, player, State::EMPTY, player, player)) {
            tripleMarkerOneEmptyBetween++;
            continue;
        }

        // 8) XX_X*  ==  tripleMarkerOneEmptyBetween
        // 9) *XX_X  ==  tripleMarkerOneEmptyBetween
        if(check4InRow(row, player, player, State::EMPTY, player)) {
            tripleMarkerOneEmptyBetween++;
            continue;
        }

        // 10) X__X*  ==  doubleMarkerTwoEmptyBetween
        // 11) *X__X  ==  doubleMarkerTwoEmptyBetween
        if(check4InRow(row, player, State::EMPTY, State::EMPTY, player)) {
            doubleMarkerTwoEmptyBetween++;
            continue;
        }

        // 12) X_X_*
        // 13) *X_X_ 
        if(check4InRow(row, player, State::EMPTY, player, State::EMPTY)) {
            doubleMarkerOneEmptyBetween++;
            continue;
        }
        
        // 14) _X_X*
        // 15) *_X_X
        if(check4InRow(row, State::EMPTY, player, State::EMPTY, player)) {
            doubleMarkerOneEmptyBetween++;
            continue;
        }

        // 16) __XX*
        // 17) *__XX
        if(check4InRow(row, State::EMPTY, State::EMPTY, player, player)) {
            doubleMarkerZeroEmptyBetween++;
            continue;
        }

        // 18) XX__*
        // 19) *XX__
        if(check4InRow(row, player, player, State::EMPTY, State::EMPTY)) {
            doubleMarkerZeroEmptyBetween++;
            continue;
        }
    }

    // counting patterns on column
    for(unsigned short col = 0; col < boardSize; col++) {
        // 1) XXXXX or XXXX* or *XXXX  ==  winning states don't appear here
        // 2) XXX** or *XXX* or **XXX  ==  losing states don't appear here

        // 3) XX_XX  ==  tripleMarkerOneEmptyBetween
        if(check5InCol(col, player, player, State::EMPTY, player, player)) {
            tripleMarkerOneEmptyBetween++;
            continue;
        }

        // 4) XX__X  ==  tripleMarkerTwoEmptyBetween
        if(check5InCol(col, player, player, State::EMPTY, State::EMPTY, player)) {
            tripleMarkerTwoEmptyBetween++;
            continue;
        }

        // 5) X__XX  ==  tripleMarkerTwoEmptyBetween
        if(check5InCol(col, player, State::EMPTY, State::EMPTY, player, player)) {
            tripleMarkerTwoEmptyBetween++;
            continue;
        }

        // 6) X_XX*  ==  tripleMarkerOneEmptyBetween
        // 7) *X_XX  ==  tripleMarkerOneEmptyBetween
        if(check4InCol(col, player, State::EMPTY, player, player)) {
            tripleMarkerOneEmptyBetween++;
            continue;
        }

        // 8) XX_X*  ==  tripleMarkerOneEmptyBetween
        // 9) *XX_X  ==  tripleMarkerOneEmptyBetween
        if(check4InCol(col, player, player, State::EMPTY, player)) {
            tripleMarkerOneEmptyBetween++;
            continue;
        }

        // 10) X__X*  ==  doubleMarkerTwoEmptyBetween
        // 11) *X__X  ==  doubleMarkerTwoEmptyBetween
        if(check4InCol(col, player, State::EMPTY, State::EMPTY, player)) {
            doubleMarkerTwoEmptyBetween++;
            continue;
        }

        // 12) X_X_*  ==  doubleMarkerOneEmptyBetween
        // 13) *X_X_  ==  doubleMarkerOneEmptyBetween
        if(check4InCol(col, player, State::EMPTY, player, State::EMPTY)) {
            doubleMarkerOneEmptyBetween++;
            continue;
        }
        
        // 14) _X_X*  ==  doubleMarkerOneEmptyBetween
        // 15) *_X_X  ==  doubleMarkerOneEmptyBetween
        if(check4InCol(col, State::EMPTY, player, State::EMPTY, player)) {
            doubleMarkerOneEmptyBetween++;
            continue;
        }

        // 16) __XX*  ==  doubleMarkerZeroEmptyBetween
        // 17) *__XX  ==  doubleMarkerZeroEmptyBetween
        if(check4InCol(col, State::EMPTY, State::EMPTY, player, player)) {
            doubleMarkerZeroEmptyBetween++;
            continue;
        }

        // 18) XX__*  ==  doubleMarkerZeroEmptyBetween
        // 19) *XX__  ==  doubleMarkerZeroEmptyBetween
        if(check4InCol(col, player, player, State::EMPTY, State::EMPTY)) {
            doubleMarkerZeroEmptyBetween++;
            continue;
        }
    }

    // counting patterns on diagonals
    State diagonalsLength5[2][5] = {
        { boardStates[0][0], boardStates[1][1], boardStates[2][2], boardStates[3][3], boardStates[4][4] },
        { boardStates[0][4], boardStates[1][3], boardStates[2][2], boardStates[3][1], boardStates[4][0] }
    };

    State diagonalsLength4[4][4] = {
        { boardStates[0][1], boardStates[1][2], boardStates[2][3], boardStates[3][4] },
        { boardStates[1][0], boardStates[2][1], boardStates[3][2], boardStates[4][3] },
        { boardStates[0][3], boardStates[1][2], boardStates[2][1], boardStates[3][0] },
        { boardStates[1][4], boardStates[2][3], boardStates[3][2], boardStates[4][1] }
    };

    
    for(unsigned short k = 0; k < 2; k++) {
        // 1) XXXXX or XXXX* or *XXXX  ==  winning states don't appear here
        // 2) XXX** or *XXX* or **XXX  ==  losing states don't appear here

        // 3) XX_XX  ==  tripleMarkerOneEmptyBetween
        if(check5InDiag5(diagonalsLength5[k], player, player, State::EMPTY, player, player)) {
            tripleMarkerOneEmptyBetween++;
            continue;
        }

        // 4) XX__X  ==  tripleMarkerTwoEmptyBetween
        if(check5InDiag5(diagonalsLength5[k], player, player, State::EMPTY, State::EMPTY, player)) {
            tripleMarkerTwoEmptyBetween++;
            continue;
        }

        // 5) X__XX  ==  tripleMarkerTwoEmptyBetween
        if(check5InDiag5(diagonalsLength5[k], player, State::EMPTY, State::EMPTY, player, player)) {
            tripleMarkerTwoEmptyBetween++;
            continue;
        }

        // 6) X_XX*  ==  tripleMarkerOneEmptyBetween
        // 7) *X_XX  ==  tripleMarkerOneEmptyBetween
        if(check4InDiag5(diagonalsLength5[k], player, State::EMPTY, player, player)) {
            tripleMarkerOneEmptyBetween++;
            continue;
        }

        // 8) XX_X*  ==  tripleMarkerOneEmptyBetween
        // 9) *XX_X  ==  tripleMarkerOneEmptyBetween
        if(check4InDiag5(diagonalsLength5[k], player, player, State::EMPTY, player)) {
            tripleMarkerOneEmptyBetween++;
            continue;
        }

        // 10) X__X*  ==  doubleMarkerTwoEmptyBetween
        // 11) *X__X  ==  doubleMarkerTwoEmptyBetween
        if(check4InDiag5(diagonalsLength5[k], player, State::EMPTY, State::EMPTY, player)) {
            doubleMarkerTwoEmptyBetween++;
            continue;
        }

        // 12) X_X_*  ==  doubleMarkerOneEmptyBetween
        // 13) *X_X_  ==  doubleMarkerOneEmptyBetween
        if(check4InDiag5(diagonalsLength5[k], player, State::EMPTY, player, State::EMPTY)) {
            doubleMarkerOneEmptyBetween++;
            continue;
        }
        
        // 14) _X_X*  ==  doubleMarkerOneEmptyBetween
        // 15) *_X_X  ==  doubleMarkerOneEmptyBetween
        if(check4InDiag5(diagonalsLength5[k], State::EMPTY, player, State::EMPTY, player)) {
            doubleMarkerOneEmptyBetween++;
            continue;
        }

        // 16) __XX*  ==  doubleMarkerZeroEmptyBetween
        // 17) *__XX  ==  doubleMarkerZeroEmptyBetween
        if(check4InDiag5(diagonalsLength5[k], State::EMPTY, State::EMPTY, player, player)) {
            doubleMarkerZeroEmptyBetween++;
            continue;
        }

        // 18) XX__*  ==  doubleMarkerZeroEmptyBetween
        // 19) *XX__  ==  doubleMarkerZeroEmptyBetween
        if(check4InDiag5(diagonalsLength5[k], player, player, State::EMPTY, State::EMPTY)) {
            doubleMarkerZeroEmptyBetween++;
            continue;
        }
    }

    for(unsigned short k = 0; k < 4; k++) {
        // 1) XXXXX or XXXX* or *XXXX  ==  winning states don't appear here
        // 2) XXX** or *XXX* or **XXX  ==  losing states don't appear here

        // 3) XX_XX  ==  tripleMarkerOneEmptyBetween    -> diagonal is too short
        // 4) XX__X  ==  tripleMarkerTwoEmptyBetween    -> diagonal is too short
        // 5) X__XX  ==  tripleMarkerTwoEmptyBetween    -> diagonal is too short
      
        // 6) X_XX*  ==  tripleMarkerOneEmptyBetween
        // 7) *X_XX  ==  tripleMarkerOneEmptyBetween
        if(check4InDiag4(diagonalsLength4[k], player, State::EMPTY, player, player)) {
            tripleMarkerOneEmptyBetween++;
            continue;
        }

        // 8) XX_X*  ==  tripleMarkerOneEmptyBetween
        // 9) *XX_X  ==  tripleMarkerOneEmptyBetween
        if(check4InDiag4(diagonalsLength4[k], player, player, State::EMPTY, player)) {
            tripleMarkerOneEmptyBetween++;
            continue;
        }

        // 10) X__X*  ==  doubleMarkerTwoEmptyBetween
        // 11) *X__X  ==  doubleMarkerTwoEmptyBetween
        if(check4InDiag4(diagonalsLength4[k], player, State::EMPTY, State::EMPTY, player)) {
            doubleMarkerTwoEmptyBetween++;
            continue;
        }

        // 12) X_X_*  ==  doubleMarkerOneEmptyBetween
        // 13) *X_X_  ==  doubleMarkerOneEmptyBetween
        if(check4InDiag4(diagonalsLength4[k], player, State::EMPTY, player, State::EMPTY)) {
            doubleMarkerOneEmptyBetween++;
            continue;
        }
        
        // 14) _X_X*  ==  doubleMarkerOneEmptyBetween
        // 15) *_X_X  ==  doubleMarkerOneEmptyBetween
        if(check4InDiag4(diagonalsLength4[k], State::EMPTY, player, State::EMPTY, player)) {
            doubleMarkerOneEmptyBetween++;
            continue;
        }

        // 16) __XX*  ==  doubleMarkerZeroEmptyBetween
        // 17) *__XX  ==  doubleMarkerZeroEmptyBetween
        if(check4InDiag4(diagonalsLength4[k], State::EMPTY, State::EMPTY, player, player)) {
            doubleMarkerZeroEmptyBetween++;
            continue;
        }

        // 18) XX__*  ==  doubleMarkerZeroEmptyBetween
        // 19) *XX__  ==  doubleMarkerZeroEmptyBetween
        if(check4InDiag4(diagonalsLength4[k], player, player, State::EMPTY, State::EMPTY)) {
            doubleMarkerZeroEmptyBetween++;
            continue;
        }
    }
    

    return {
        movementFreedom, possibleWins, possibleOpponentLoses, winLosePositions, 
        doubleMarkerZeroEmptyBetween, doubleMarkerOneEmptyBetween, doubleMarkerTwoEmptyBetween, 
        tripleMarkerOneEmptyBetween, tripleMarkerTwoEmptyBetween
    };
}

bool Board::check4InRow(unsigned short row, State marker1, State marker2, State marker3, State marker4) {
    for(unsigned short k = 0; k < 2; k++) {
        if(boardStates[row][0 + k] == marker1 &&
           boardStates[row][1 + k] == marker2 &&
           boardStates[row][2 + k] == marker3 &&
           boardStates[row][3 + k] == marker4) { return true; }
    }
    
    return false;
}
    
bool Board::check5InRow(unsigned short row, State marker1, State marker2, State marker3, State marker4, State marker5) {
    return (boardStates[row][0] == marker1 &&
            boardStates[row][1] == marker2 &&
            boardStates[row][2] == marker3 &&
            boardStates[row][3] == marker4 &&
            boardStates[row][4] == marker5);
}

bool Board::check4InCol(unsigned short col, State marker1, State marker2, State marker3, State marker4) {
    for(unsigned short k = 0; k < 2; k++) {
        if(boardStates[0 + k][col] == marker1 &&
           boardStates[1 + k][col] == marker2 &&
           boardStates[2 + k][col] == marker3 &&
           boardStates[3 + k][col] == marker4) { return true; }
    }
    
    return false;
}
    
bool Board::check5InCol(unsigned short col, State marker1, State marker2, State marker3, State marker4, State marker5) {
    return (boardStates[0][col] == marker1 &&
            boardStates[1][col] == marker2 &&
            boardStates[2][col] == marker3 &&
            boardStates[3][col] == marker4 &&
            boardStates[4][col] == marker5);
}

bool Board::check4InDiag4(State* diagonal, State marker1, State marker2, State marker3, State marker4) {
    return (diagonal[0] == marker1 &&
            diagonal[1] == marker2 &&
            diagonal[2] == marker3 &&
            diagonal[3] == marker4);
}

bool Board::check4InDiag5(State* diagonal, State marker1, State marker2, State marker3, State marker4) {
    for(unsigned short k = 0; k < 2; k++) {
        if(diagonal[0 + k] == marker1 &&
           diagonal[1 + k] == marker2 &&
           diagonal[2 + k] == marker3 &&
           diagonal[3 + k] == marker4) { return true; }
    }

    return false;
}

bool Board::check5InDiag5(State* diagonal, State marker1, State marker2, State marker3, State marker4, State marker5) {
    return (diagonal[0] == marker1 &&
            diagonal[1] == marker2 &&
            diagonal[2] == marker3 &&
            diagonal[3] == marker4 &&
            diagonal[4] == marker5);
}