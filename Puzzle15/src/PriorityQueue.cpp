#include "PriorityQueue.h"

PriorityQueue::PriorityQueue() : heap{nullptr} { }

void PriorityQueue::push(State* node) {
    heap.push_back(node);
    node->heapIdx = heap.size() - 1;

    siftUp(heap.size() - 1);
}

State* PriorityQueue::pop() {
    if(!empty()) {
        State* node = heap[1];
        swap(1, heap.size() - 1);

        heap.pop_back();
        if (1 < heap.size()) {
            siftDown(1);
        }

        return node;
    }

    return nullptr;
}

void PriorityQueue::remove(State* node) {
    size_t idx = node->heapIdx;
    swap(idx, heap.size() - 1);

    heap.pop_back();
    if (idx < heap.size()) {
        siftUp(idx);
        siftDown(idx);
    }
}

void PriorityQueue::update(State* node) {
    size_t idx = node->heapIdx;
    siftUp(idx);
    siftDown(idx);
}

bool PriorityQueue::empty() const { return heap.size() == 1; }

size_t PriorityQueue::size() const { return heap.size() - 1; }

void PriorityQueue::swap(size_t idx1, size_t idx2) {
    State* tmp = heap[idx1];
    heap[idx1] = heap[idx2];
    heap[idx2] = tmp;
    heap[idx1]->heapIdx = idx1;
    heap[idx2]->heapIdx = idx2;
}

void PriorityQueue::siftUp(size_t idx) {
    size_t parentIdx = idx / 2;
    if ((parentIdx >= 1) && (*(heap[idx]) < *(heap[parentIdx]))) {
        swap(idx, parentIdx);
        siftUp(parentIdx);
    }
}

void PriorityQueue::siftDown(size_t idx) {
    size_t childIdx = idx;
    if ((2*idx < heap.size()) && (*(heap[2*idx]) < *(heap[childIdx]))) {
        childIdx = 2*idx;
    }
    if ((2*idx+1 < heap.size()) && (*(heap[2*idx+1]) < *(heap[childIdx]))) {
        childIdx = 2*idx+1;
    }
    if (idx != childIdx) {
        swap(idx, childIdx);
        siftDown(childIdx);
    }
}