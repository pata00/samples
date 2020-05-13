#ifndef _CO_FUNCTION_H__
#define _CO_FUNCTION_H__

#include <cassert>
#include <exception>
#include <type_traits>

//https://docs.microsoft.com/zh-cn/cpp/preprocessor/predefined-macros?view=vs-2019
#ifdef _MSC_VER//msvc编译器
	#if _MSC_VER >= 1925
		#include <experimental/coroutine>
		namespace co_operator {
			using suspend_always = std::experimental::suspend_always;
		};

		namespace co_function_detail {
			template<typename T>
			using coroutine_handle = typename std::experimental::coroutine_handle<T>;
		}
	#else
		#error "msvc编译器版本过低"
	#endif
#else
	//输出g++预定义宏 gcc -x c++ /dev/null -dM -E -std=c++17
	#ifdef __GNUC__
		#if (__GNUC__ >= 10) && (__GNUC_MINOR__ >= 1)
			#include <coroutine>
			namespace co_operator {
				using suspend_always = std::suspend_always;
			};

			namespace co_function_detail {
				template<typename T>
				using coroutine_handle = typename std::coroutine_handle<T>;
			};
		#else
			#error "g++ 版本过低"
		#endif
	#else
		#error "未知的编译器"
	#endif
#endif


template <typename T>
class co_function {
public:
	class promise_type {
	public:
		T storage_val_;
		auto get_return_object() {
			return co_function_detail::coroutine_handle<promise_type>::from_promise(*this);
		}

		auto initial_suspend() {
			return co_operator::suspend_always();
		}

		void return_value(T&& val) {
			storage_val_ = std::move(val);
		}

		void unhandled_exception() {
			std::terminate();
		}

		auto final_suspend() noexcept {
			return co_operator::suspend_always();
		}
	};

	co_function(co_function_detail::coroutine_handle<promise_type> handle)
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

	void resume() {
		assert(handle_);
		assert(!handle_.done());
		handle_.resume();
	}

	bool done() const {
		assert(handle_);
		return handle_.done();
	}

	const T& get_ret_value() const {
		return handle_.promise().storage_val_;
	}

	~co_function() noexcept {
		if (handle_) {
			handle_.destroy();
		}
	}
private:
	co_function_detail::coroutine_handle<promise_type> handle_;
};

template <>
class co_function<void> {
public:
	struct promise_type {
		auto get_return_object() {
			return co_function_detail::coroutine_handle<promise_type>::from_promise(*this);//协程函数有额外的栈空间
		}

		auto initial_suspend() {
			return co_operator::suspend_always();		//协程函数创建之后不自动运行
		}

		void return_void() {

		}

		void unhandled_exception() {
			std::terminate();
		}

		auto final_suspend() noexcept {
			return co_operator::suspend_always();
		}
	};

	co_function(co_function_detail::coroutine_handle<promise_type> handle)
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

	void resume() {
		assert(handle_);
		assert(!handle_.done());
		handle_.resume();
	}

	bool done() const {
		assert(handle_);
		return handle_.done();
	}

	~co_function() noexcept {
		if (handle_) {
			handle_.destroy();
		}
	}
private:
	co_function_detail::coroutine_handle<promise_type> handle_;
};


#endif // !_CO_FUNCTION_H__
