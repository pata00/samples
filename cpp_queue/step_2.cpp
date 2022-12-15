#include <cassert>
#include <cstdio>
#include <deque>
#include <memory>
#include <shared_mutex>
#include <string>
#include <thread>

std::shared_mutex shared_mtx;
std::deque<std::string> queue;

constexpr const char* msg = "can you out loop";

void thread_recv() {
    printf("thread_recv begin loop\n");
    while (true) {

        auto lg = std::shared_lock(shared_mtx);
        if (!queue.empty()) {
            break;
        }
    }
    printf("thread_recv end loop\n");
    auto str = std::move(queue.front());
    assert(str == msg);
    {
        auto lg = std::unique_lock(shared_mtx);
        queue.pop_front();
    }
    printf("thread_recv get msg:%s\n", str.c_str());
}

void thread_send() {
    std::this_thread::sleep_for(std::chrono::seconds(1));
    {
        auto lg = std::unique_lock(shared_mtx);
        queue.emplace_back(msg);
    }
    printf("thread_send finished\n");
}

int main(int argc, char* argv[]) {
    std::thread t1(thread_recv);
    std::thread t2(thread_send);
    t1.join();
    t2.join();
    return 0;
}

/**
 * shared_mtx 封装的是 pthread_rwlock_t
 */