//##===----------------------------------------------------------------------===##//
//
//  Author: Nathan Phipps 1/19/19
//
//##===----------------------------------------------------------------------===##//

#ifndef NP_ENGINE_RANDOM_BASE_HPP
#define NP_ENGINE_RANDOM_BASE_HPP

#include <fstream>
#include <type_traits>

#include "NP-Engine/Foundation/Foundation.hpp"
#include "NP-Engine/Primitive/Primitive.hpp"
#include "NP-Engine/FileSystem/FileSystem.hpp"

#include "RandomEngine.hpp"

namespace np::random
{
	template <typename T>
	class RandomBase
	{
	private:
		NP_ENGINE_STATIC_ASSERT((::std::is_same_v<T, Random32> || ::std::is_same_v<T, Random64>),
								"RandomBase<T> requires T to be random32 or random64");

		T _original_random_engine;
		T _random_engine;

	public:
		RandomBase(): _original_random_engine(T())
		{
			ResetRandomEngine();
		}

		RandomBase(const T& engine): _original_random_engine(engine)
		{
			Init();
		}

		inline T GetOriginalRandomEngine() const
		{
			return _original_random_engine;
		}

		inline T& GetRandomEngine()
		{
			return _random_engine;
		}

		inline void SetRandomEngine(const T& engine)
		{
			_original_random_engine = engine;
			_random_engine = _original_random_engine;
		}

		inline void ResetRandomEngine()
		{
			_random_engine = _original_random_engine;
		}

		virtual inline void Init()
		{
			ResetRandomEngine();
		}
	};

	typedef RandomBase<Random32> Random32Base;
	typedef RandomBase<Random64> Random64Base;
} // namespace np::random

#endif /* NP_ENGINE_RANDOM_BASE_HPP */
