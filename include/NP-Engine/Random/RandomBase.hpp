//
//  RandomBase.hpp
//  Project Space
//
//  Created by Nathan Phipps on 1/19/19.
//  Copyright © 2019 Nathan Phipps. All rights reserved.
//

#ifndef NP_ENGINE_RANDOM_BASE_HPP
#define NP_ENGINE_RANDOM_BASE_HPP

#include <type_traits>

#include "NP-Engine/Primitive/Primitive.hpp"
#include "NP-Engine/Insight/Insight.hpp"
#include "NP-Engine/Serialization/Serialization.hpp"
#include "NP-Engine/Filesystem/Filesystem.hpp"

#include "RandomEngine.hpp"

namespace np
{
	namespace random
	{
		/**
		 random32base is to be used for objects that require a random32 engine
		 */
		template <typename T>
		class RandomBase : public serialization::Serializable
		{
		private:
			NP_STATIC_ASSERT((::std::is_same_v<T, Random32> || ::std::is_same_v<T, Random64>),
							 "RandomBase<T> requires T to be random32 or random64");

			T _original_random_engine;
			T _random_engine;

		public:
			/**
			 constructor
			 sets random engines to new engine using random32
			 */
			RandomBase(): _original_random_engine(T())
			{
				ResetRandomEngine();
			}

			/**
			 constructor
			 sets random engines to given random engine
			 */
			RandomBase(const T& engine): _original_random_engine(engine)
			{
				Init();
			}

			/**
			 deconstructor
			 */
			virtual ~RandomBase() {}

			/**
			 gets the original random engine
			 */
			inline T GetOriginalRandomEngine() const
			{
				return _original_random_engine;
			}

			/**
			 gets the random engine that started as the original random engine
			 */
			inline T& GetRandomEngine()
			{
				return _random_engine;
			}

			/**
			 sets the original random engine and the random engine
			 the original random engine will be kept as is so we can always known where we started
			 */
			inline void SetRandomEngine(const T& engine)
			{
				_original_random_engine = engine;
				_random_engine = _original_random_engine;
			}

			/**
			 resets the random engine back to the original state
			 */
			inline void ResetRandomEngine()
			{
				_random_engine = _original_random_engine;
			}

			/**
			 serialization method for us to write
			 we require our objects to know which filepath they are a part of
			 */
			virtual ostrm& Insertion(ostrm& os, str filepath) const override
			{
				using StateType = typename T::StateType;
				using SeedType = typename T::SeedType;
				ui8 state_type_size = sizeof(StateType);
				ui8* bytes = new ui8[state_type_size];

				// write _original_random_engine
				SeedType seed = _original_random_engine.GetSeed();
				for (i32 i = 0; i < state_type_size; i++)
				{
					bytes[i] = (seed.GetInc() >> (i * 8)) & 255;
				}
				os.write((chr*)bytes, state_type_size);

				for (i32 i = 0; i < state_type_size; i++)
				{
					bytes[i] = (seed.GetState() >> (i * 8)) & 255;
				}
				os.write((chr*)bytes, state_type_size);

				// write _random_engine
				seed = _random_engine.GetSeed();
				for (i32 i = 0; i < state_type_size; i++)
				{
					bytes[i] = (seed.GetInc() >> (i * 8)) & 255;
				}
				os.write((chr*)bytes, state_type_size);

				for (i32 i = 0; i < state_type_size; i++)
				{
					bytes[i] = (seed.GetState() >> (i * 8)) & 255;
				}
				os.write((chr*)bytes, state_type_size);

				delete[] bytes;
				return os;
			}

			/**
			 deserialization method for us to read
			 we require our objects to know which filepath they are a part of
			 */
			virtual istrm& Extraction(istrm& is, str filepath) override
			{
				using StateType = typename T::StateType;
				using SeedType = typename T::SeedType;
				ui8 state_type_size = sizeof(StateType);
				ui8* bytes = new ui8[state_type_size];
				StateType inc = 0;
				StateType state = 0;
				SeedType seed;

				// read _original_random_engine
				inc = 0;
				state = 0;

				is.read((chr*)bytes, state_type_size);
				for (i32 i = 0; i < state_type_size; i++)
				{
					inc += (StateType)bytes[i] << (i * 8);
				}

				is.read((chr*)bytes, state_type_size);
				for (i32 i = 0; i < state_type_size; i++)
				{
					state += (StateType)bytes[i] << (i * 8);
				}

				seed.SetInc(inc);
				seed.SetState(state);
				_original_random_engine.SetSeed(seed);

				// read _random_engine
				inc = 0;
				state = 0;
				is.read((chr*)bytes, state_type_size);
				for (i32 i = 0; i < state_type_size; i++)
				{
					inc += (StateType)bytes[i] << (i * 8);
				}

				is.read((chr*)bytes, state_type_size);
				for (i32 i = 0; i < state_type_size; i++)
				{
					state += (StateType)bytes[i] << (i * 8);
				}

				seed.SetInc(inc);
				seed.SetState(state);
				_random_engine.SetSeed(seed);

				delete[] bytes;
				return is;
			}

			/**
			 save oursellves inside the given dirpath
			 return if the save was successful or not
			 */
			virtual bl SaveTo(str dirpath) const override
			{
				str filename = "Random" + to_str(sizeof(typename T::result_type) * 8) + "base.bin";
				return RandomBase<T>::template SaveAs<RandomBase<T>>(fs::Append(dirpath, filename), this);
			}

			/**
			 load outselves from the given dirpath
			 return if the load was successful or not
			 */
			virtual bl LoadFrom(str dirpath) override
			{
				str filename = "Random" + to_str(sizeof(typename T::result_type) * 8) + "base.bin";
				return RandomBase<T>::template LoadAs<RandomBase<T>>(fs::Append(dirpath, filename), this);
			}

			/**
			 initializes the noise object -- should be done after you call SetRandomEngine
			 */
			virtual inline void Init()
			{
				ResetRandomEngine();
			}
		};

		typedef RandomBase<Random32> Random32Base;
		typedef RandomBase<Random64> Random64Base;
	} // namespace random
} // namespace np

#endif /* NP_ENGINE_RANDOM_BASE_HPP */
