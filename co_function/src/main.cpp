#include <iostream>
#include "co_function.h"



//co_await co_return co_yield原理
//https://blog.panicsoftware.com/your-first-coroutine/


co_function test_co_function() {
    std::cout << u8"enter test_fun02" << std::endl;
    co_await std::experimental::suspend_always();
    std::cout << u8"enter test_fun02" << std::endl;
    co_return 1;
}


int main(int argc, char* argv[])
{
    auto t = test_co_function();
    while (!t.done()) {
        t.resume();
    };

    auto ret = t.get_ret_value();

    std::cout << u8"test_co_function return :" << ret << std::endl;
    return 0;
}