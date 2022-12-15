#include <boost/thread/shared_mutex.hpp>
#include <cassert>
#include <condition_variable>
#include <cstdio>
#include <deque>
#include <mutex>
#include <string>
#include <thread>

std::mutex read_mtx;
std::mutex write_mtx;

std::condition_variable_any cv;
std::deque<std::string> queue;

void thread_recv(int count) {
    printf("thread_recv begin loop\n");
    int busy_loop_cnt = 0;

    for (int i = 0; i < count || count == 0; ++i) {

        auto rlk = std::unique_lock(read_mtx);

        cv.wait(rlk, [&] {
            return !queue.empty() || !(++busy_loop_cnt);
        });

        auto wlk = std::unique_lock(write_mtx);

        auto str = std::move(queue.front());
        queue.pop_front();

        rlk.unlock();
        wlk.unlock();
        

        assert(str == std::to_string(i));
    }
    printf("thread_recv finished busy_loop_cnt = %d\n", busy_loop_cnt);
}

void thread_send(int count) {
    for (int i = 0; i < count || count == 0; ++i) {
        {
            auto lk = std::unique_lock(write_mtx);
            queue.emplace_back(std::to_string(i));
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