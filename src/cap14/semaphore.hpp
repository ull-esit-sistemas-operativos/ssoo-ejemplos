// semaphore.hpp -  Clase de semáforo
//

#pragma once

#include <mutex>
#include <condition_variable>

namespace examples
{
    class semaphore {
    public:
        
        semaphore(int count = 0) : count_(count) {}

        void release() {
            std::unique_lock<std::mutex> lock(mutex_);
            count_++;
            cv_.notify_one();
        }

        void acquire() {
            std::unique_lock<std::mutex> lock(mutex_);
            while(count_ == 0) {
                cv_.wait(lock);
            }
            count_--;
        }

    private:
        std::mutex mutex_;
        std::condition_variable cv_;
        int count_;
    };
} // namespace examples