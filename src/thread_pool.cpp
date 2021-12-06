#include "thread_pool.h"
#include <stdio.h>
#include <assert.h>


ThreadPool::ThreadPool(size_t threadCount) : m_stop(false) 
{
    for (size_t i = 0; i < threadCount; i++) {
        m_threads.emplace_back(Worker, this);
    }
}

ThreadPool::~ThreadPool() 
{
    Stop();    
}

void ThreadPool::Worker() 
{
    printf("Worker !\n");
    while (true) {
        std::function<void()> task;
        {
            std::unique_lock lock(m_mutex);
            m_onEvent.wait(lock, [&]() { return m_stop || !m_taskQueue.empty(); });
            
            if (m_stop) {
                return;
            }
            assert(!m_taskQueue.empty());
            task = std::move(m_taskQueue.front());
            m_taskQueue.pop();
        }
        task();
    }    
}

template <typename T>
std::future<T> ThreadPool::Enqueue(std::function<T()> task) 
{
    auto wrapper = std::make_shared<std::packaged_task<T()>>(
        std::move(task)
    );
    {
        std::lock_guard lock(m_mutex);
        m_taskQueue.emplace([=] {
            (*wrapper)();
        });
    }
    m_onEvent.notify_one();
    return wrapper->get_future();
}


void ThreadPool::Stop() 
{
    {
        std::lock_guard lock(m_mutex);
        m_stop = true;
    }
    m_onEvent.notify_all();
    {
        std::lock_guard lock(m_mutex);
        for (auto& t : m_threads) {
            t.join();
        }
        m_threads.clear();
    }
}