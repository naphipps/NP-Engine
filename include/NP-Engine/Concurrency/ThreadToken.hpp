//##===----------------------------------------------------------------------===##//
//
//  Author: Nathan Phipps 8/27/20
//
//##===----------------------------------------------------------------------===##//

#ifndef NP_ENGINE_THREAD_TOKEN_HPP
#define NP_ENGINE_THREAD_TOKEN_HPP

#include "NP-Engine/Primitive/Primitive.hpp"

#include "Thread.hpp"

namespace np::concurrency
{
	/*
		contains a Thread pointer so outside objects cannot manipulate it
	*/
	class ThreadToken
	{
	private:
		Thread* _thread;

	public:
		ThreadToken(): _thread(nullptr) {}

		ThreadToken(Thread* thread): _thread(thread) {}

		ThreadToken(const ThreadToken& token): _thread(token._thread) {}

		ThreadToken(ThreadToken&& token) noexcept: _thread(token._thread) {}

		inline bl IsValid() const
		{
			return _thread != nullptr;
		}

		void Invalidate()
		{
			_thread = nullptr;
		}

		inline Thread& GetThread()
		{
			NP_ENGINE_ASSERT(IsValid(), "we require a valid token when getting the thread");
			return *_thread;
		}

		inline const Thread& GetThread() const
		{
			NP_ENGINE_ASSERT(IsValid(), "we require a valid token when getting the thread");
			return *_thread;
		}

		ThreadToken& operator=(const ThreadToken& token)
		{
			_thread = token._thread;
			return *this;
		}

		ThreadToken& operator=(ThreadToken&& token) noexcept
		{
			_thread = token._thread;
			return *this;
		}

		bl operator==(const ThreadToken& other) const
		{
			return _thread == other._thread;
		}
	};
} // namespace np::concurrency

#endif /* NP_ENGINE_THREAD_TOKEN_HPP */
