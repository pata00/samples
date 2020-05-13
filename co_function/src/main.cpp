#include <iostream>
#include "co_function.h"
#include <vector>


//co_await co_return co_yield原理参考
//https://blog.panicsoftware.com/category/evolution/coroutines/


co_function<void> co_function1() {
    int a = 0;
    std::cout << u8"enter co_function1 " << ++a << std::endl;
    co_await co_operator::suspend_always();
    std::cout << u8"enter co_function1 " << ++a << std::endl;
    co_await co_operator::suspend_always();
    std::cout << u8"enter co_function1 " << ++a << std::endl;
    co_return;
}

co_function<int> co_function2() {
    int a = 0;
    std::cout << u8"enter co_function2 " << ++a << std::endl;
    co_await co_operator::suspend_always();
    std::cout << u8"enter co_function2 " << ++a << std::endl;
    co_await co_operator::suspend_always();
    std::cout << u8"enter co_function2 " << ++a << std::endl;
    co_return std::move(a);
}

co_function<std::vector<int>> co_function3() {
    std::vector<int> ret;
    ret.emplace_back(1);
    std::cout << u8"enter co_function3 " << ret.size() <<std::endl;
    co_await co_operator::suspend_always();
    ret.emplace_back(2);
    std::cout << u8"enter co_function3 " << ret.size() <<std::endl;
    co_await co_operator::suspend_always();
    ret.emplace_back(3);
    std::cout << u8"enter co_function3 " << ret.size() <<std::endl;
    co_return std::move(ret);
}


int main(int argc, char* argv[])
{
    auto t1 = co_function1();
    while (!t1.done()) {
        t1.resume();
    };

    auto t2 = co_function2();
    while (!t2.done()) {
        t2.resume();
    };
    auto r2 = t2.get_ret_value();

    auto t3 = co_function3();
    while (!t3.done()) {
        t3.resume();
    };
    auto r3 = t3.get_ret_value();
    assert(r3.size() == 3);
    return 0;
}