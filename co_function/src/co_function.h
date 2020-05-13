#ifndef _CO_FUNCTION_H__
#define _CO_FUNCTION_H__

#include <cassert>
#include <experimental/coroutine>
#include <type_traits>

class co_function {
public:
	struct promise_type {
		int ret_;
		auto get_return_object() {
			return std::experimental::coroutine_handle<promise_type>::from_promise(*this);//协程函数有额外的栈空间
		}


		auto initial_suspend() {
			return std::experimental::suspend_always();		//协程函数创建之后不自动运行
		}

		//协程函数中调用co_return;则实现return_void如果调用了co_return expr;则会调用相应类型的return_value(expr);
		void return_value(int ret) {
			ret_ = ret;
		}

		void unhandled_exception() {
			std::terminate();
		}

		auto final_suspend() {
			return std::experimental::suspend_always();	//保证最后调用done()的时候能返回true
		}
	};

	//被编译器调用
	co_function(std::experimental::coroutine_handle<promise_type> handle)
		: handle_(handle)
	{
		assert(handle);
		static_assert(sizeof(handle_) == 8);
	}

	co_function(co_function&) = delete;

	co_function(co_function&& obj) noexcept
		: handle_(obj.handle_)
	{
		obj.handle_ = nullptr;
	}

	//继续执行函数
	void resume() {
		assert(handle_);
		assert(!handle_.done());
		handle_.resume();
	}

	//只能在本协程函数外判断，如果在协程函数里调用永远返回false
	bool done() const {
		assert(handle_);
		return handle_.done();
	}

	int get_ret_value() const {
		return handle_.promise().ret_;
	}

	~co_function() noexcept {
		if (handle_) {
			handle_.destroy();
		}
	}
private:
	std::experimental::coroutine_handle<promise_type> handle_;
};

#endif // !_CO_FUNCTION_H__