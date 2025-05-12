#pragma once

#include <vector>
#include <queue>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <functional>

struct Task {
    float priority;
    std::function<void()> func;

    // For the priority queue to sort tasks (lowest first, for example)
    bool operator<(const Task& other) const {
        return priority > other.priority;
    }
};

class PriorityThreadPool {
public:
    PriorityThreadPool(size_t numThreads);
    ~PriorityThreadPool();

    void enqueueTask(const Task& task);

private:
    std::vector<std::thread> workers;
    std::priority_queue<Task> tasks;

    std::mutex queueMutex;
    std::condition_variable condition;
    bool stop;

    void workerThread();
};
