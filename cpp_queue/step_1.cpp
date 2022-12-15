#include <cassert>
#include <cstdio>
#include <deque>
#include <memory>
#include <mutex>
#include <string>
#include <thread>

std::mutex mtx;
std::deque<std::string> queue;

constexpr const char* msg = "can you out loop";

void thread_recv() {
    printf("thread_recv begin loop\n");
    while (true) {
        //std::atomic_thread_fence(std::memory_order_consume);
        std::atomic_thread_fence(std::memory_order_seq_cst);
        if (!queue.empty()) {
            break;
        }
    }
    printf("thread_recv end loop\n");
    auto str = std::move(queue.front());
    assert(str == msg);
    mtx.lock();
    queue.pop_front();
    mtx.unlock();
    printf("thread_recv get msg:%s\n", str.c_str());
}

void thread_send() {
    std::this_thread::sleep_for(std::chrono::seconds(1));
    mtx.lock();
    queue.emplace_back(msg);
    std::atomic_thread_fence(std::memory_order_release);
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


/**
 * 6种内存序
 * memory_order_relaxed     最松散的内存序,只能用来保护原子变量自身，无法保护上下文
 * memory_order_consume     相当于只对依赖数据做memory_order_acquire
 * memory_order_acquire     等价于lfence, 但是amd64下并不直接调用该指令，而是通过生成指令等效实现，防止load指令穿越至屏障上方
 * memory_order_release     等价于sfence, 但是amd64下并不直接调用该指令，而是通过生成指令等效实现，防止save指令穿越至屏障下方
 * memory_order_acq_rel     相当于同时指定memory_order_acquire和memory_order_release
 * memory_order_seq_cst     等价于mfence，最严格的内存序，并且amd64下实现就是调用mfence指令
 */