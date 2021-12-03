#pragma once
#include "algorithms/diamond.h"


// The queue doesn't store the diamonds,
// only pointers to them.
// It uses internal fields in the diamond class :
// queueNext and queuePrev.
// We always assume a diamond is at most in a single queue at any time.
class DiamondQueue
{
public:
    inline const static uint32_t NO_QUEUE_ID = 0;

    DiamondQueue(uint32_t id);
    ~DiamondQueue();

    uint32_t GetID() const; 

    void Remove(Diamond* d);
    void AddFirst(Diamond* d);
    void AddLast(Diamond* d);
    // d is added right before curr,
    // then curr is set to d.
    void AddBeforeCurrent(Diamond* d);

    void AdvanceCurrent();
    Diamond* GetCurrent();
    const Diamond* GetFirst() const;
    void SetCurrentToFirst();
private:
    uint32_t m_id;
    Diamond* m_first;
    Diamond* m_last;
    Diamond* m_curr;
};