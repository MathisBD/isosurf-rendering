#pragma once
#include <thread>
#include <mutex>
#include <condition_variable>
#include <vector>
#include <queue>
#include <functional>
#include <future>


class ThreadPool
{
public:
    ThreadPool(size_t threadCount);
    ~ThreadPool();

    template <typename T>
    std::future<T> Enqueue(std::function<T()> task);
private:
    std::mutex m_mutex;
    std::condition_variable m_onEvent;
    bool m_stop;

    std::vector<std::thread> m_threads;
    std::queue<std::function<void()>> m_taskQueue;

    void Start();
    void Stop();
    
    void Worker();
};

