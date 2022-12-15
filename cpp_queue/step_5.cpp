#include <cassert>
#include <condition_variable>
#include <cstdio>
#include <deque>
#include <mutex>
#include <string>
#include <thread>

std::condition_variable cv;
std::mutex mtx;
std::deque<std::string> queue;

// 信号丢失， wait之前已经调用notify，则信号会丢失
// 虚假唤醒， 没有任何人调用notify，但是wait也可能会返回

void thread_recv(int count) {
    printf("thread_recv begin loop\n");
    int busy_loop_cnt = 0;

    for (int i = 0; i < count || count == 0; ++i) {

        auto lk = std::unique_lock(mtx);

        // 手写循环版本
        // while (queue.empty()) {
        //     busy_loop_cnt++;
        //     cv.wait(lk);
        // }

        // lambda性能理论上限更高，因为减少了cv.wait函数的调用次数
        // https://stackoverflow.com/questions/13722426/why-can-lambdas-be-better-optimized-by-the-compiler-than-plain-functions
        cv.wait(lk, [&] {
            return !queue.empty() || !(++busy_loop_cnt);
        });

        auto str = std::move(queue.front());
        queue.pop_front();

        lk.unlock();

        assert(str == std::to_string(i));
    }
    printf("thread_recv finished busy_loop_cnt = %d\n", busy_loop_cnt);
}

void thread_send(int count) {
    for (int i = 0; i < count || count == 0; ++i) {
        {
            auto lg = std::unique_lock(mtx);
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