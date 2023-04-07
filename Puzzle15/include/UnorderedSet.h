#ifndef UNORDERED_SET_H
#define UNORDERED_SET_H

#include <unordered_set>

#include "State.h"


struct StateCompare {
public:
    bool operator() (const State* state1, const State* state2) const {
        return state1->representation == state2->representation;
    }
};

struct StateHash {
public:
    size_t operator()(const State* state) const {
        return state->hashValue;
	}
};


typedef std::unordered_set<State*, StateHash, StateCompare> UnorderedSet;


#endif  // UNORDERED_SET_H 