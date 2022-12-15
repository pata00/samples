#include <cassert>
#include <condition_variable>
#include <cstdio>
#include <deque>
// #include <memory>
#include "pthread.h"
#include <shared_mutex>
#include <string>
#include <thread>

class rwlock {
  public:
    rwlock() {
        pthread_rwlock_init(&_lock, nullptr);
    }

    ~rwlock() {
        pthread_rwlock_destroy(&_lock);
    }

    void read_lock() {
        pthread_rwlock_rdlock(&_lock);
    }

    void write_lock() {
        pthread_rwlock_wrlock(&_lock);
    }

    void lock() {
        read_lock();
    }

    void unlock() {
        pthread_rwlock_unlock(&_lock);
    }

  private:
    pthread_rwlock_t _lock;
};

rwlock lock;
std::condition_variable_any cv;
std::deque<std::string> queue;

void thread_recv(int count) {
    printf("thread_recv begin loop\n");
    int busy_loop_cnt = 0;

    for (int i = 0; i < count || count == 0; ++i) {

        auto readlock = std::unique_lock(lock);
        cv.wait(readlock, [&] {
            return !queue.empty() || !(++busy_loop_cnt);
        });
        readlock.unlock();
        // need readlock upgrade to write lock, otherwise poor performance.
        // rwlock only work for one consumer because below can lead to dead lock
        // t1 hold readlock
        // t2 hold readlock
        // t1 query upgrade lock
        // t2 query upgrade lock
        // ...
        // 
        lock.write_lock();
        auto str = std::move(queue.front());
        queue.pop_front();
        lock.unlock();

        assert(str == std::to_string(i));
    }
    printf("thread_recv finished busy_loop_cnt = %d\n", busy_loop_cnt);
}

void thread_send(int count) {
    for (int i = 0; i < count || count == 0; ++i) {
        {
            lock.write_lock();
            queue.emplace_back(std::to_string(i));
            lock.unlock();
        }
        cv.notify_one();
    }
    printf("thread_send finished\n");
}

int main(int argc, char* argv[]) {
    int count = 10000;
    if (argc >= 2) {
        count = std::atoi(argv[1]);
    }
    printf("count is %d\n", count);

    std::thread t1(thread_recv, count);
    std::thread t2(thread_send, count);
    t1.join();
    t2.join();
    return 0;
}