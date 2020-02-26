#include <iostream>
#include <cassert>
#include <experimental/coroutine>

//co_await co_return co_yield原理
//https://blog.panicsoftware.com/your-first-coroutine/

#pragma region my_resumable01
class my_resumable01 {
public:
	struct promise_type {
		using coro_handle = std::experimental::coroutine_handle<promise_type>;
		auto get_return_object() {
			return coro_handle::from_promise(*this);
		}
		auto initial_suspend() { return std::experimental::suspend_always(); }
		auto final_suspend() { return std::experimental::suspend_always(); }
		void return_void() {}
		void unhandled_exception() {
			std::terminate();
		}

		auto yield_value(int* v) {

		}
	};

	my_resumable01(promise_type::coro_handle handle) : handle_(handle) { assert(handle); }
	my_resumable01(my_resumable01&) = delete;
	my_resumable01(my_resumable01&&) = delete;
	bool resume() {
		if (!handle_.done())
			handle_.resume();
		return !handle_.done();
	}
	~my_resumable01() { handle_.destroy(); }
private:
	promise_type::coro_handle handle_;
};

my_resumable01 test_fun01() {
    std::cout << u8"enter test_fun1" << std::endl;
    co_await std::experimental::suspend_always();
    std::cout << u8"enter test_fun2" << std::endl;
    co_await std::experimental::suspend_always();
    std::cout << u8"enter test_fun3" << std::endl;
    co_await std::experimental::suspend_always();
    std::cout << u8"enter test_fun4" << std::endl;
}
#pragma endregion

#pragma region my_resumable02
class my_resumable02 {
public:
	struct promise_type {
		using coro_handle = std::experimental::coroutine_handle<promise_type>;
		auto get_return_object() {
			return coro_handle::from_promise(*this);
		}
		auto initial_suspend() { return std::experimental::suspend_always(); }
		auto final_suspend() { return std::experimental::suspend_always(); }

		int  return_value_;
		void return_value(int a) {//test_fun02要实现co_return，则必须实现此接口
			return_value_ = a;
		}
		void unhandled_exception() {
			std::terminate();
		}

		int yield_value_;
		auto yield_value(int v) {//test_fun02中如果要使用co_yield，则必须实现此接口
			yield_value_ = v;
			return std::experimental::suspend_always();
		}
	};

	my_resumable02(promise_type::coro_handle handle) : handle_(handle) { assert(handle); }
	my_resumable02(my_resumable02&) = delete;
	my_resumable02(my_resumable02&&) = delete;
	bool resume() {
		if (!handle_.done())
			handle_.resume();
		return !handle_.done();
	}
	~my_resumable02() { handle_.destroy(); }
	int recent_val() {
		return handle_.promise().yield_value_;
	}

	int return_val() {
		return handle_.promise().return_value_;
	}
private:
	promise_type::coro_handle handle_;
};

my_resumable02 test_fun02() {
	std::cout << u8"enter test_fun1" << std::endl;
	co_await std::experimental::suspend_always();
	std::cout << u8"enter test_fun2" << std::endl;
	co_await std::experimental::suspend_always();
	std::cout << u8"enter test_fun3" << std::endl;
	co_await std::experimental::suspend_always();
	std::cout << u8"enter test_fun4" << std::endl;

	int enter_cnt = 0;
	while (enter_cnt < 10)
		co_yield ++enter_cnt;
	co_return 10000;	//如果调用了co_return my_resumable02::promise_type  必须实现return_value，替换return_void
}
#pragma endregion

int main(int argc, char* argv[])
{
	#pragma region test my_resumable01 最简单的c++20 coroutine，只支持在函数中使用co_await
	{
		my_resumable01 co_handle = test_fun01();
		int call_cnt = 0;
		while (true) {
			call_cnt++;
			std::cout << u8"begin call_cnt:" << call_cnt << std::endl;
			bool need_contine = co_handle.resume();
			if (!need_contine)
				break;
		}
	}
	#pragma endregion

	#pragma region test my_resumable02 在01例子的基础上增加co_yield，co_return支持
	{
		my_resumable02 co_handle = test_fun02();
		int call_cnt = 0;
		while (true) {
			call_cnt++;
			std::cout << u8"begin call_cnt:" << call_cnt << std::endl;
			bool need_contine = co_handle.resume();

			if (!need_contine) {
				int r_value = co_handle.return_val();
				std::cout << u8"get return value" << r_value << std::endl;
				break;
			}	
			int y_value = co_handle.recent_val();
			std::cout << u8"get yield value" << y_value << std::endl;
		}
	}
	#pragma endregion
    return 0;
}