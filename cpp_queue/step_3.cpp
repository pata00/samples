#include <cassert>
#include <cstdio>
#include <deque>
#include <memory>
#include <shared_mutex>
#include <string>
#include <thread>

std::shared_mutex shared_mtx;
std::deque<std::string> queue;

void thread_recv(int count) {
    printf("thread_recv begin loop\n");
    int busy_loop_cnt = 0;

    for (int i = 0; i < count || count == 0; ++i) {
        while (true) {
            std::atomic_thread_fence(std::memory_order_consume);
            if (!queue.empty()) {
                break;
            }
            busy_loop_cnt++;
        }
        auto str = std::move(queue.front());
        assert(str == std::to_string(i)); 
// macos下，不管是consume还是acquire，都会触发上述断言，只能通过读写锁来同步
// 估计跟其stl实现有关，盲猜queue.emplace_back的实现，没有彻底做完就修改了size导致queue.emtpy()先通过了判断，导致后续读数据的时候对面还没写完
// 测试环境为
// Apple clang version 14.0.0 (clang-1400.0.29.202)
// Target: x86_64-apple-darwin22.1.0
// Thread model: posix
// InstalledDir: /Library/Developer/CommandLineTools/usr/bin
        {
            auto lg = std::unique_lock(shared_mtx);
            queue.pop_front();
        }
    }
    printf("thread_recv finished busy_loop_cnt = %d\n", busy_loop_cnt);
}

void thread_send(int count) {
    for (int i = 0; i < count || count == 0; ++i) {
        auto lg = std::unique_lock(shared_mtx);
        queue.emplace_back(std::to_string(i));
        std::atomic_thread_fence(std::memory_order_release);
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

/**
 * shared_mtx 封装的是 pthread_rwlock_t
 */
