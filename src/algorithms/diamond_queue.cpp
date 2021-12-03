#include "algorithms/diamond_queue.h"
#include <stdio.h>


DiamondQueue::DiamondQueue(uint32_t id) 
{
    assert(id != NO_QUEUE_ID);
    m_id = id;
    m_first = m_last = m_curr = nullptr;
}

DiamondQueue::~DiamondQueue() 
{    
}

uint32_t DiamondQueue::GetID() const
{
    return m_id;    
}

void DiamondQueue::AdvanceCurrent() 
{
    assert(m_curr);
    m_curr = m_curr->queueNext;    
}

Diamond* DiamondQueue::GetCurrent() 
{
    return m_curr;    
}

const Diamond* DiamondQueue::GetFirst() const
{
    return m_first;    
}

void DiamondQueue::SetCurrentToFirst() 
{
    m_curr = m_first;    
}


void DiamondQueue::AddFirst(Diamond* d) 
{
    assert(d->queueID == NO_QUEUE_ID);
    if (!m_first) {
        assert(!m_last && !m_curr);
        m_first = m_last = m_curr = d;
        d->queueNext = d->queuePrev = nullptr;
    }    
    else {
        assert(m_last);
        d->queueNext = m_first;
        d->queuePrev = nullptr;
        m_first->queuePrev = d;
        m_first = d;
    }
    d->queueID = m_id;

    assert((m_first && m_last) || 
           (!m_first && !m_last));
}

void DiamondQueue::AddLast(Diamond* d) 
{
    assert(d->queueID == NO_QUEUE_ID);
    if (!m_first) {
        assert(!m_last && !m_curr);
        m_first = m_last = m_curr = d;
        d->queueNext = d->queuePrev = nullptr;
    }    
    else {
        assert(m_last);
        d->queuePrev = m_last;
        d->queueNext = nullptr;
        m_last->queueNext = d;
        m_last = d;
    }
    d->queueID = m_id;

    assert((m_first && m_last) || 
           (!m_first && !m_last));
}

void DiamondQueue::AddBeforeCurrent(Diamond* d) 
{
    assert(d->queueID == NO_QUEUE_ID);
    assert(m_curr && m_first && m_last);
    if (m_first == m_curr) {
        m_first = d;
    }
    d->queueNext = m_curr;
    d->queuePrev = m_curr->queuePrev;
    
    m_curr->queuePrev->queueNext = d;
    m_curr->queuePrev = d;

    m_curr = d;
    d->queueID = m_id;
}

void DiamondQueue::Remove(Diamond* d) 
{
    assert(d->queueID == m_id);
    if (m_curr == d) {
        m_curr = m_curr->queueNext;
    }
    if (m_first == d) {
        m_first = d->queueNext;
    }    
    if (m_last == d) {
        m_last = d->queuePrev;
    }

    if (d->queueNext) {
        d->queueNext->queuePrev = d->queuePrev;
    }
    if (d->queuePrev) {
        d->queuePrev->queueNext = d->queueNext;
    }
    d->queuePrev = d->queueNext = nullptr;
    d->queueID = NO_QUEUE_ID;

    assert((m_first && m_last) || 
           (!m_first && !m_last));
}
