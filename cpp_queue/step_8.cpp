#include <cassert>
#include <cstdio>
#include <deque>
#include <mutex>
#include <semaphore.h>
#include <string>
#include <thread>

std::mutex mtx;
sem_t sem;
std::deque<std::string> queue;

// semaphore 只是一个计数器，如果数据结构本身不是无锁的，还是得配合mutex使用
// 此时性能不如条件变量队列

void thread_recv(int count) {
    printf("thread_recv begin loop\n");

    for (int i = 0; i < count || count == 0; ++i) {
        sem_wait(&sem);
        auto lg = std::unique_lock(mtx);
        auto str = std::move(queue.front());
        queue.pop_front();
        lg.unlock();
        assert(str == std::to_string(i));
    }
    printf("thread_recv finished busy_loop_cnt = %d\n", 0);
}

void thread_send(int count) {
    for (int i = 0; i < count || count == 0; ++i) {
        auto lg = std::unique_lock(mtx);
        queue.emplace_back(std::to_string(i));
        lg.unlock();
        sem_post(&sem);
    }
    printf("thread_send finished\n");
}

int main(int argc, char* argv[]) {
    int count = 10000;
    if (argc >= 2) {
        count = std::atoi(argv[1]);
    }
    printf("count is %d\n", count);

    sem_init(&sem, 0, 0);

    std::thread t1(thread_recv, count);
    std::thread t2(thread_send, count);
    t1.join();
    t2.join();
    return 0;
}