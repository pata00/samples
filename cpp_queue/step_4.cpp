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


void thread_recv(int count) {
    printf("thread_recv begin loop\n");
    int busy_loop_cnt = 0;

    for (int i = 0; i < count || count == 0; ++i) {
        while(true){
            auto lg = std::shared_lock(shared_mtx);
            if(!queue.empty()){
                break;
            }
            busy_loop_cnt++;
        }
        auto str = std::move(queue.front());
        assert(str == std::to_string(i));
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
    }
    printf("thread_send finished\n");
}

int main(int argc, char* argv[]) {
    int count = 10000;
    if(argc >= 2){
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