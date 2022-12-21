
#include <cassert>
#include <cstdio>
#include <string>
#include <thread>

#include "3rd/readerwriterqueue/readerwriterqueue.h"

moodycamel::BlockingReaderWriterQueue<std::string> queue;

// semaphore 只是一个计数器，如果数据结构本身不是无锁的，还是得配合mutex使用
// 此时性能不如条件变量队列

void thread_recv(int count) {
    printf("thread_recv begin loop\n");

    for (int i = 0; i < count || count == 0; ++i) {
        std::string str;
        queue.wait_dequeue(str);
        assert(str == std::to_string(i));
    }
    printf("thread_recv finished busy_loop_cnt = %d\n", 0);
}

void thread_send(int count) {
    for (int i = 0; i < count || count == 0; ++i) {
        queue.emplace(std::to_string(i));
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