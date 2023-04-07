#ifndef PRIORITY_QUEUE_H
#define PRIORITY_QUEUE_H

#include <vector>

#include "State.h"

class PriorityQueue {
private:
    std::vector<State*> heap;

public:
    PriorityQueue();

    void push(State* node);
    State* pop();
    void remove(State* node);
    void update(State* node);

    bool empty() const;
    size_t size() const;

private:
    void swap(size_t idx1, size_t idx2);
    void siftUp(size_t idx);
    void siftDown(size_t idx);
};

#endif  // PRIORITY_QUEUE_H