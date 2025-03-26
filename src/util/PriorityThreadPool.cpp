#include "PriorityThreadPool.hpp"

/**
 * @brief Construct a new Thread Pool:: Thread Pool object
 * 
 * @param numThreads The number of threads to instantiate
 */
PriorityThreadPool::PriorityThreadPool(size_t numThreads) : stop(false) {
    for (size_t i = 0; i < numThreads; i++) {
        workers.emplace_back([this]() { workerThread(); });
    }
}

/**
 * @brief Destroy the Thread Pool:: Thread Pool object
 * 
 */
PriorityThreadPool::~PriorityThreadPool() {
    {
        std::unique_lock<std::mutex> lock(queueMutex);
        stop = true;
    }
    condition.notify_all();
    for (std::thread &worker : workers) {
        worker.join();
    }
}

/**
 * @brief Enqueue a new task onto the thread pool
 * 
 * @param task Function to be called
 */
void PriorityThreadPool::enqueueTask(const Task& task) {
    {
        std::unique_lock<std::mutex> lock(queueMutex);
        tasks.push(std::move(task));
    }
    condition.notify_one();
}

void PriorityThreadPool::workerThread() {
    while (true) {
        Task task;
        {
            std::unique_lock<std::mutex> lock(queueMutex);
            condition.wait(lock, [this] { return stop || !tasks.empty(); });

            if (stop && tasks.empty()) return;

            task = std::move(tasks.top());
            tasks.pop();
        }
        task.func();
    }
}
