#include "Puzzle15.h"

Puzzle15::Puzzle15() : 
    startState{}, 
    generator(std::random_device{}()) 
{
    std::vector<short> puzzlesOrder{0x1, 0x2, 0x3, 0x4, 0x5, 0x6, 0x7, 0x8, 
        0x9, 0xA, 0xB, 0xC, 0xD, 0xE, 0xF, 0x0};

    startState = State(puzzlesOrder);
}

Puzzle15::Puzzle15(ull initialState) : 
    startState{initialState}, 
    generator(std::random_device{}()) { } 

bool Puzzle15::canBeSolved() const {
    short index = startState.getEmptyPuzzelIndex();
    short row = index / 4;

    if(State::size % 2 == 0) {
        if(row % 2 == 1) {
            return startState.inversionNumber() % 2 == 0;
        }
        else {
            return startState.inversionNumber() % 2 == 1;
        }
    }
    else {
        return startState.inversionNumber() % 2 == 0;
    }
}

void Puzzle15::shuffle(short swapsNumber) {
    std::vector<short> initialPuzzle15sOrder{0x1, 0x2, 0x3, 0x4, 0x5, 0x6, 0x7, 0x8, 
        0x9, 0xA, 0xB, 0xC, 0xD, 0xE, 0xF, 0x0};
    
    startState = State{initialPuzzle15sOrder};

    std::set<ull> oldStates;
    oldStates.insert(startState.representation);

    std::vector<ull> neighbors;
    while(swapsNumber > 0) {
        neighbors = startState.generateNeighbors();
        std::uniform_int_distribution<short> dis(0, static_cast<short>(neighbors.size() - 1));
        short neighborIndex = dis(generator);

        if(std::find(oldStates.cbegin(), oldStates.cend(), neighbors[neighborIndex]) == oldStates.cend()) {
            oldStates.insert(neighbors[neighborIndex]);
            startState = State{neighbors[neighborIndex]};
            swapsNumber--;
        }
    }    
}

void Puzzle15::randomShuffle() {
    std::vector<short> puzzlesOrder{0x1, 0x2, 0x3, 0x4, 0x5, 0x6, 0x7, 0x8, 
        0x9, 0xA, 0xB, 0xC, 0xD, 0xE, 0xF};

    // fisher-yates-shuffle-algorithm
    for(short index1 = 14; index1 > 0; index1--) {
        std::uniform_int_distribution<short> dis(0, index1);
        short index2 = dis(generator);
 
        short tmp = puzzlesOrder[index1];
        puzzlesOrder[index1] = puzzlesOrder[index2];
        puzzlesOrder[index2] = tmp;
    }

    puzzlesOrder.push_back(0x0);

    startState = State(puzzlesOrder);
}

void Puzzle15::findSolution() {
    // A* algorithm

    PriorityQueue openNodes;
    UnorderedSet allNodes; 

    ull rounds = 0;
    
    openNodes.push(&startState);
    allNodes.insert(&startState);

    while(!openNodes.empty()) {
        State* current = openNodes.pop();

        if(current->representation == 0x0FEDCBA987654321) {
            std::vector<State*> moves;
            while(current != nullptr) {
                moves.push_back(current);
                current = current->previousStage;
            }

            std::for_each(moves.rbegin(), moves.rend(),
            [](State* move){
                std::cout << *move << '\n';
            }); 
                
            std::cout << rounds << ',' << moves.size() - 1 << '\n';
          
            return;
        }
        
        ull newGValue = current->gValue + 1;
        
        for(const ull& state : current->generateNeighbors()) {
            State* newState = new State(state);

            UnorderedSet::iterator it = allNodes.find(newState);
            if(it == allNodes.end()) {
                allNodes.insert(newState);
                newState->isOnOpenList = true;
                newState->previousStage = current;
                newState->gValue = newGValue;
                openNodes.push(newState);

                rounds++;
            }
            else {
                if(newGValue < (*it)->gValue) {
                    (*it)->previousStage = current;
                    (*it)->gValue = newGValue;

                    if((*it)->isOnOpenList) {
                        openNodes.update(*it);
                    }
                    else {
                        openNodes.push(*it);

                        rounds++;
                    }
                }
            }
        }
    }
}

std::ostream& operator<<(std::ostream& stream, const Puzzle15& puzzle) {
    std::cout << puzzle.startState;

    return stream;
}