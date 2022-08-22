//##===----------------------------------------------------------------------===##//
//
//  Author: Nathan Phipps 12/7/20
//
//##===----------------------------------------------------------------------===##//

#ifndef NP_ENGINE_RANDOM_SEED_HPP
#define NP_ENGINE_RANDOM_SEED_HPP

#include <type_traits>

#include "NP-Engine/Foundation/Foundation.hpp"
#include "NP-Engine/Primitive/Primitive.hpp"

#include "PcgRandutils.hpp"

namespace np::rng
{
	template <typename T>
	class RandomSeed
	{
	private:
		NP_ENGINE_STATIC_ASSERT((::std::is_same_v<T, ui64> || ::std::is_same_v<T, ::pcg_extras::pcg128_t>),
								"RandomSeed<T> requires T to be ui64 or ::pcg_extras::pcg128_t");

		T _inc;
		T _state;

		void ForceOddInc()
		{
			_inc |= 1;
		}

		void CopyFrom(const RandomSeed<T>& other)
		{
			_inc = other._inc;
			_state = other._state;
		}

	public:
		RandomSeed(): RandomSeed(0, 0) {} // TODO: figure out better default values here

		RandomSeed(T inc, T state): _inc(inc), _state(state)
		{
			ForceOddInc();
		}

		RandomSeed(const RandomSeed<T>& seed)
		{
			CopyFrom(seed);
		}

		RandomSeed(RandomSeed<T>&& seed)
		{
			CopyFrom(seed);
		}

		RandomSeed<T>& operator=(const RandomSeed<T>& other)
		{
			CopyFrom(other);
			return *this;
		}

		RandomSeed<T>& operator=(RandomSeed<T>&& other)
		{
			CopyFrom(other);
			return *this;
		}

		T GetInc() const
		{
			return _inc;
		}

		T GetState() const
		{
			return _state;
		}

		void SetInc(T inc)
		{
			_inc = inc;
			ForceOddInc();
		}

		void SetState(T state)
		{
			_state = state;
		}
	};
} // namespace np::rng

#endif /* NP_ENGINE_RANDOM_SEED_HPP */
