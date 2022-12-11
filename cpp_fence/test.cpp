#include <cstdio>
#include <deque>
#include <memory>
#include <mutex>
#include <string>
#include <thread>

std::mutex mtx;
std::deque<std::string> queue;

void thread_recv() {
    printf("thread_recv begin loop\n");
    while (queue.empty()) {
        // without atomic_thread_fence test on platform amd64
        //  compile -O0 -O1 -O2 -O3
        //  g++-9    Y   N   N   N
        //  g++-10   Y   N   Y   Y
        //  g++-12   Y   N   Y   Y

        // std::atomic_thread_fence(std::memory_order_consume);
    }
    printf("thread_recv end loop\n");
    mtx.lock();
    auto str = std::move(queue.front());
    queue.pop_front();
    mtx.unlock();
}

void thread_send() {
    std::this_thread::sleep_for(std::chrono::seconds(1));
    mtx.lock();
    queue.emplace_back("can you out loop");
    mtx.unlock();
    printf("thread_send finished\n");
}

int main(int argc, char* argv[]) {
    std::thread t1(thread_recv);
    std::thread t2(thread_send);
    t1.join();
    t2.join();
    return 0;
}
