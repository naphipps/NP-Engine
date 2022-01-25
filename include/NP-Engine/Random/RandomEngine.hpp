//##===----------------------------------------------------------------------===##//
//
//  Author: Nathan Phipps 9/12/20
//
//##===----------------------------------------------------------------------===##//

#ifndef NP_ENGINE_RANDOM_ENGINE_HPP
#define NP_ENGINE_RANDOM_ENGINE_HPP

#include <type_traits>

#include "NP-Engine/Insight/Insight.hpp"
#include "NP-Engine/Container/Container.hpp"
#include "NP-Engine/Primitive/Primitive.hpp"

#include "PcgRandutils.hpp"
#include "RandomSeed.hpp"

namespace np::random
{
	template <typename T>
	class RandomEngine : public T
	{
	private:
		NP_STATIC_ASSERT((::std::is_same_v<T, ::pcg32> || ::std::is_same_v<T, ::pcg64>),
						 "random_engine<T> requires T to be ::pcg32 or ::pcg64");

	public:
		using ResultType = typename T::result_type;
		using StateType = typename T::state_type;
		using SeedType = RandomSeed<typename T::state_type>;

	private:
		void CopyFrom(const T& other)
		{
			RandomEngine<T> engine(other);
			CopyFrom(engine);
		}

		void CopyFrom(const RandomEngine<T>& other)
		{
			SetIncAndState(other.GetInc(), other.GetState());
		}

	public:
		RandomEngine(): T()
		{
			Init();
		}

		RandomEngine(const RandomEngine<T>& engine): T(engine) {}

		RandomEngine(RandomEngine<T>&& engine): T(engine) {}

		RandomEngine(const T& engine): T(engine) {}

		RandomEngine(T&& engine): T(engine) {}

		RandomEngine(const SeedType& seed): T()
		{
			SetSeed(seed);
		}

		RandomEngine& operator=(const RandomEngine<T>& other)
		{
			CopyFrom(other);
			return *this;
		}

		RandomEngine& operator=(RandomEngine<T>&& other)
		{
			CopyFrom(other);
			return *this;
		}

		RandomEngine& operator=(const T& other)
		{
			CopyFrom(other);
			return *this;
		}

		RandomEngine& operator=(T&& other)
		{
			CopyFrom(other);
			return *this;
		}

		void Init()
		{
			if constexpr (::std::is_same_v<T, ::pcg32>)
			{
				*this = ::pcg32_fe().engine();
			}
			else // if constexpr (::std::is_same_v<T, ::pcg64>)
			{
				*this = ::pcg64_fe().engine();
			}
		}

		ResultType GetLemireWithinRange(ResultType range)
		{
			ResultType x = (*this)();
			StateType m = StateType(x) * StateType(range);
			ResultType l = ResultType(m);

			if (l < range)
			{
				ResultType t = -range;
				if (t >= range)
				{
					t -= range;
					if (t >= range)
					{
						t %= range;
					}
				}
				while (l < t)
				{
					x = (*this)();
					m = StateType(x) * StateType(range);
					l = ResultType(m);
				}
			}

			StateType shift_amount = 0;

			if constexpr (::std::is_same_v<T, ::pcg32>)
			{
				shift_amount = 32;
			}
			else // if constexpr (::std::is_same_v<T, ::pcg64>)
			{
				shift_amount = 64;
			}

			return (ResultType)(m >> shift_amount);
		}

		template <siz SIZE>
		ResultType GetRouletteWheelSelection(const container::array<ResultType, SIZE>& distribution)
		{
			// add all the weights to determine range
			ResultType range = 0;
			for (ui64 i = 0; i < distribution.size(); i++)
			{
				range += distribution[i];
			}

			// random result from range
			ResultType result = GetLemireWithinRange(range);
			ResultType index = 0;

			// determine index from result
			for (ui64 i = 0; i < distribution.size(); i++)
			{
				if (result < distribution[i])
				{
					index = (ResultType)i;
					break;
				}
				result -= distribution[i];
			}

			return index;
		}

		StateType GetMult() const
		{
			return T::multiplier();
		}

		StateType GetInc() const
		{
			return T::inc_;
		}

		StateType GetState() const
		{
			return T::state_;
		}

		void SetInc(StateType inc)
		{
			T::inc_ = inc;
		}

		void SetState(StateType state)
		{
			T::state_ = state;
		}

		void SetIncAndState(StateType inc, StateType state)
		{
			SetInc(inc);
			SetState(state);
		}

		SeedType CreateSeed()
		{
			StateType shift_amount = 0;

			if constexpr (::std::is_same_v<T, ::pcg32>)
			{
				shift_amount = 32;
			}
			else // if constexpr (::std::is_same_v<T, ::pcg64>)
			{
				shift_amount = 64;
			}

			// generate new inc
			T::inc_ = (*this)();
			T::inc_ = T::inc_ << shift_amount;
			T::inc_ |= (*this)();

			// inc always needs to be odd
			T::inc_ |= 1;

			// generate new state
			T::state_ = (*this)();
			T::state_ = T::state_ << shift_amount;
			T::state_ |= (*this)();

			SeedType seed(GetInc(), GetState());
			return seed;
		}

		SeedType GetSeed() const
		{
			return SeedType(GetInc(), GetState());
		}

		void SetSeed(const SeedType& seed)
		{
			SetIncAndState(seed.GetInc(), seed.GetState());
		}
	};

	using Random32 = RandomEngine<::pcg32>;
	using Random64 = RandomEngine<::pcg64>;
	using Random32Seed = Random32::SeedType;
	using Random64Seed = Random64::SeedType;
} // namespace np::random

#endif /* NP_ENGINE_RANDOM_ENGINE_HPP */
