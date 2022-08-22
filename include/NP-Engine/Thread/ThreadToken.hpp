//##===----------------------------------------------------------------------===##//
//
//  Author: Nathan Phipps 8/27/20
//
//##===----------------------------------------------------------------------===##//

#ifndef NP_ENGINE_THREAD_TOKEN_HPP
#define NP_ENGINE_THREAD_TOKEN_HPP

#include <utility>

#include "NP-Engine/Primitive/Primitive.hpp"

#include "ThreadImpl.hpp"

namespace np::thr
{
	class ThreadToken
	{
	private:
		Thread* _thread;

	public:
		ThreadToken(): _thread(nullptr) {}

		ThreadToken(Thread* thread): _thread(thread) {}

		ThreadToken(const ThreadToken& token): _thread(token._thread) {}

		ThreadToken(ThreadToken&& token) noexcept: _thread(::std::move(token._thread)) {}

		ThreadToken& operator=(const ThreadToken& token)
		{
			_thread = token._thread;
			return *this;
		}

		ThreadToken& operator=(ThreadToken&& token) noexcept
		{
			_thread = ::std::move(token._thread);
			return *this;
		}

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
			return *_thread;
		}

		inline const Thread& GetThread() const
		{
			return *_thread;
		}

		bl operator==(const ThreadToken& other) const
		{
			return _thread == other._thread;
		}
	};
} // namespace np::thr

#endif /* NP_ENGINE_THREAD_TOKEN_HPP */
