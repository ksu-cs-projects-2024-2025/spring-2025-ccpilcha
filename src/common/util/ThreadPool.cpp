#include "ThreadPool.hpp"
#include <iostream>

/**
 * @brief Construct a new Thread Pool:: Thread Pool object
 * 
 * @param numThreads The number of threads to instantiate
 */
ThreadPool::ThreadPool(size_t numThreads) : stop(false) {
    for (size_t i = 0; i < numThreads; i++) {
        workers.emplace_back([this]() { workerThread(); });
    }
}

/**
 * @brief Destroy the Thread Pool:: Thread Pool object
 * 
 */
ThreadPool::~ThreadPool() {
    {
        std::unique_lock<std::mutex> lock(queueMutex);
        stop = true;
    }
    std::cout << "Stopping thread pool" << std::endl;
    condition.notify_all();
    for (std::thread &worker : workers) {
        worker.join();
    }
    std::cout << "Thread pool stopped" << std::endl;
}

/**
 * @brief Enqueue a new task onto the thread pool
 * 
 * @param task Function to be called
 */
void ThreadPool::enqueueTask(std::function<void()> task) {
    {
        std::unique_lock<std::mutex> lock(queueMutex);
        tasks.push(std::move(task));
    }
    condition.notify_one();
}

void ThreadPool::workerThread() {
    while (true) {
        std::function<void()> task;
        {
            std::unique_lock<std::mutex> lock(queueMutex);
            condition.wait(lock, [this] { return stop || !tasks.empty(); });

            if (stop) return;

            task = std::move(tasks.front());
            tasks.pop();
        }
        task();
    }
}
