//
//  RandomEngine.hpp
//  Project Space
//
//  Created by Nathan Phipps on 9/12/20.
//  Copyright © 2020 Nathan Phipps. All rights reserved.
//

#ifndef NP_ENGINE_RANDOM_ENGINE_HPP
#define NP_ENGINE_RANDOM_ENGINE_HPP

#include <type_traits>

#include "NP-Engine/Insight/Insight.hpp"
#include "NP-Engine/Container/Container.hpp"
#include "NP-Engine/Primitive/Primitive.hpp"

#include "PcgRandutils.hpp"
#include "RandomSeed.hpp"

namespace np
{
	namespace random
	{
		/**
		 random engine
		 */
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
			/**
			 copies from other random_engine base
			 */
			void CopyFrom(const T& other)
			{
				RandomEngine<T> engine(other);
				CopyFrom(engine);
			}

			/**
			 copies from other random_engine
			 */
			void CopyFrom(const RandomEngine<T>& other)
			{
				SetIncAndState(other.GetInc(), other.GetState());
			}

		public:
			/**
			 constructor
			 */
			RandomEngine(): T()
			{
				Reconstruct();
			}

			/**
			 constructor
			 */
			RandomEngine(const RandomEngine<T>& engine): T(engine) {}

			/**
			 constructor
			 */
			RandomEngine(RandomEngine<T>&& engine): T(engine) {}

			/**
			 constructor
			 */
			RandomEngine(const T& engine): T(engine) {}

			/**
			 constructor
			 */
			RandomEngine(T&& engine): T(engine) {}

			/**
			 constructor
			 */
			RandomEngine(const SeedType& seed): T()
			{
				SetSeed(seed);
			}

			/**
			 deconstructor
			 */
			~RandomEngine() {}

			/**
			 copy assignment
			 */
			RandomEngine& operator=(const RandomEngine<T>& other)
			{
				CopyFrom(other);
				return *this;
			}

			/**
			 move assignment -- acts just like copy
			 */
			RandomEngine& operator=(RandomEngine<T>&& other)
			{
				CopyFrom(other);
				return *this;
			}

			/**
			 copy assignment
			 */
			RandomEngine& operator=(const T& other)
			{
				CopyFrom(other);
				return *this;
			}

			/**
			 move assignment -- acts just like copy
			 */
			RandomEngine& operator=(T&& other)
			{
				CopyFrom(other);
				return *this;
			}

			/**
			 reconstructor
			 */
			void Reconstruct()
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

			/**
			 performs Lemire's optimized debiased–integer-multiplication method with some edits by Melissa O'Neill 2018-07-22
			 output: [0, range)
			 */
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

			/**
			 performs Roulette Wheel Selection on the given distribution
			 */
			template <siz SIZE>
			ResultType GetRouletteWheelSelection(const container::array<ui32, SIZE>& distribution)
			{
				// TODO: feel like this should have it's own distribution file

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

			/**
			 gets the multiplier value
			 */
			StateType GetMult() const
			{
				return T::multiplier();
			}

			/**
			 gets the inc value
			 */
			StateType GetInc() const
			{
				return T::inc_;
			}

			/**
			 gets the state value
			 */
			StateType GetState() const
			{
				return T::state_;
			}

			/**
			 sets the inc value
			 */
			void SetInc(StateType inc)
			{
				T::inc_ = inc;
			}

			/**
			 sets the state value
			 */
			void SetState(StateType state)
			{
				T::state_ = state;
			}

			/**
			 sets the inc and state
			 */
			void SetIncAndState(StateType inc, StateType state)
			{
				SetInc(inc);
				SetState(state);
			}

			/**
			 creates new seed depending on the current state of this engine
			 */
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

			/**
			 gets the seed
			 */
			SeedType GetSeed() const
			{
				return SeedType(GetInc(), GetState());
			}

			/**
			 sets the seed
			 */
			void SetSeed(const SeedType& seed)
			{
				SetIncAndState(seed.GetInc(), seed.GetState());
			}
		};

		typedef RandomEngine<::pcg32> Random32;
		typedef RandomEngine<::pcg64> Random64;
		typedef Random32::SeedType Random32seed;
		typedef Random64::SeedType Random64seed;
	} // namespace random
} // namespace np

// force the usage of pcg_extras operators for pcg128 use cases with streams
using ::pcg_extras::operator<<;
using ::pcg_extras::operator>>;

#endif /* NP_ENGINE_RANDOM_ENGINE_HPP */
