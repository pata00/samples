#ifndef _CO_FUNCTION_H__
#define _CO_FUNCTION_H__

#include <cassert>
#include <exception>
#include <type_traits>

//https://docs.microsoft.com/zh-cn/cpp/preprocessor/predefined-macros?view=vs-2019
#ifdef _MSC_VER//msvc编译器
	#if _MSC_VER >= 1925
#include <experimental/coroutine>
template <typename T>
class co_function {
	template<typename P>
	using coroutine_handle = typename std::experimental::coroutine_handle<P>;
	using suspend_always = std::experimental::suspend_always;
public:
	struct promise_type {
		auto get_return_object() {
			return coroutine_handle<promise_type>::from_promise(*this);//协程函数有额外的栈空间
		}

		auto initial_suspend() {
			return suspend_always();		//协程函数创建之后不自动运行
		}

		template <typename R = T>
		std::enable_if_t<std::is_void<R>::value, void>
		return_void() {
			//printf("call return void\n");
		}

		//template <typename R = T>
		//std::enable_if_t<!std::is_void<R>::value, void>
		//return_void() {
		//	printf("call return void222\n");
		//}

		//template <typename R = T>
		//std::enable_if_t<!std::is_void<R>::value, void>
		//	return_value(R r) {
		//	//ret_ = ret;
		//}

		void unhandled_exception() {
			std::terminate();
		}

		auto final_suspend() {
			return suspend_always();	//保证最后调用done()的时候能返回true
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

	//T get_ret_value() const {
	//	return handle_.promise().ret_;
	//}

	~co_function() noexcept {
		if (handle_) {
			handle_.destroy();
		}
	}
private:
	coroutine_handle<promise_type> handle_;
};
	#else
		#error "msvc编译器版本过低"
	#endif
#else
	//输出g++预定义宏 gcc -x c++ /dev/null -dM -E -std=c++17
	#ifdef __GNUC__
		#if (__GNUC__ >= 10) && (__GNUC_MINOR__ >= 1)
#include <coroutine>
template <typename T>
class co_function {
	template<typename P>
	using coroutine_handle = typename std::coroutine_handle<P>;
	using suspend_always = std::suspend_always;
public:
	struct promise_type {
		auto get_return_object() {
			return coroutine_handle<promise_type>::from_promise(*this);//协程函数有额外的栈空间
		}

		auto initial_suspend() {
			return suspend_always();		//协程函数创建之后不自动运行
		}

		template <typename R = T>
		std::enable_if_t<std::is_void<R>::value, void>
			return_void() {
			//printf("call return void\n");
		}

		//template <typename R = T>
		//std::enable_if_t<!std::is_void<R>::value, void>
		//return_void() {
		//	printf("call return void222\n");
		//}

		//template <typename R = T>
		//std::enable_if_t<!std::is_void<R>::value, void>
		//	return_value(R r) {
		//	//ret_ = ret;
		//}

		void unhandled_exception() {
			std::terminate();
		}

		auto final_suspend() {
			return suspend_always();	//保证最后调用done()的时候能返回true
		}
	};

	//被编译器调用
	co_function(std::coroutine_handle<promise_type> handle)
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

	//T get_ret_value() const {
	//	return handle_.promise().ret_;
	//}

	~co_function() noexcept {
		if (handle_) {
			handle_.destroy();
		}
	}
private:
	coroutine_handle<promise_type> handle_;
};
		#else
			#error "g++ 版本过低"
		#endif
	#else
		#error "未知的编译器"
	#endif
#endif

#endif // !_CO_FUNCTION_H__