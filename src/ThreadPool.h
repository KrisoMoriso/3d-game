#pragma once
#include <condition_variable>
#include <functional>
#include <queue>
#include <thread>
#include <vector>


class ThreadPool {
public:

    ThreadPool(size_t threads);
    ~ThreadPool();
    void enqueue(std::function<void()> task);
private:
    bool stop;
    std::vector<std::thread> workers;
    std::queue<std::function<void()>> tasks;
    std::mutex queue_mutex;
    std::condition_variable condition;
};
