#include <deque>
#include <string>
#include <thread>
#include <mutex>
#include <cstdio>
#include <memory>

std::mutex mtx;
std::deque<std::string> test_queue;

void thread_recv(){
    printf("thread_recv begin loop\n");
    while (test_queue.empty()) {
        //std::atomic_thread_fence(std::memory_order_acquire);  without this line, gcc-9, gcc-10 with -O1 will deal loop
    }
    printf("thread_recv end loop\n");
    mtx.lock();
    auto str = std::move(test_queue.front());
    test_queue.pop_front();
    mtx.unlock();
}

void thread_send(){
    std::this_thread::sleep_for(std::chrono::seconds(1));
    mtx.lock();
    test_queue.emplace_back("can you out loop");
    mtx.unlock();
    printf("thread_send finished\n");
}

int main(int argc, char *argv[]){
    std::thread t1(thread_recv);
    std::thread t2(thread_send);
    t1.join();
    t2.join();
    return 0;
}
