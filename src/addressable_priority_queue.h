#ifndef ADDRESSABLE_PRIORITY_QUEUE_H_INCLUDED
#define ADDRESSABLE_PRIORITY_QUEUE_H_INCLUDED

template<typename T, typename Compare, typename Adjust>
class AddressablePriorityQueue
{
public:
    std::vector<int> p;                         // Position of unique identifier in APQ
    std::vector<std::pair<int,T>> v;          // First component is current vertex(unique identifier), second component is the key
    Compare comp;
    Adjust adjust;
    AddressablePriorityQueue(int n);
    void insertElement(int current, T key);
    void siftUp(int index);
    void siftDown(int index);
    int deleteRoot();

    void updateKey(int current, T value);

    void print() const;
};

#include "addressable_priority_queue.tpp"

#endif // ADDRESSABLE_PRIORITY_QUEUE_H_INCLUDED
