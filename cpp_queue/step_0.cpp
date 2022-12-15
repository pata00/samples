#include <cstdio>
#include <deque>
#include <mutex>
#include <string>
#include <thread>

std::mutex mtx;
std::deque<std::string> queue;

constexpr const char* msg = "can you out loop";

void thread_recv() {
    printf("thread_recv begin loop\n");
    while (queue.empty()) {
        //  compile -O0 -O1 -O2 -O3
        //  g++-9    Y   N   N   N
        //  g++-10   Y   N   Y   Y
        //  g++-12   Y   N   Y   Y
        //  对比其生成的汇编代码，总结如下:
        //    -O0无优化:循环中每次都从内存读取数据,正确运行没毛病
        //    -O1优化为:循环外读取内存数据到寄存器,循环中只比较寄存器的值,故无法感知其他核对内存的修改导致死循环
        //    -O2 -O3 在高版本编译中因为循环内什么都没做，相当于循环直接被优化成空，所以没有死循环了,
        //       但是会有内存错误,因为queue为空的状态下取了front,打印顺序可以验证，用valgrind也可以检测得到
    }
    printf("thread_recv end loop\n");
    mtx.lock();
    auto str = std::move(queue.front());
    assert(str == msg);
    queue.pop_front();
    mtx.unlock();
    printf("thread_recv get msg:%s\n", str.c_str());
}

void thread_send() {
    std::this_thread::sleep_for(std::chrono::seconds(1));
    mtx.lock();
    queue.emplace_back(msg);
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
