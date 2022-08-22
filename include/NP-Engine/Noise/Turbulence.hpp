//
//  Turbulence.hpp
//  Project Space
//
//  Created by Nathan Phipps on 4/26/20.
//  Copyright © 2020 Nathan Phipps. All rights reserved.
//

#ifndef NP_ENGINE_TURBULENCE_HPP
#define NP_ENGINE_TURBULENCE_HPP

#include "NP-Engine/Primitive/Primitive.hpp"
#include "NP-Engine/Random/Random.hpp"
#include "NP-Engine/String/String.hpp"
#include "NP-Engine/FileSystem/FileSystem.hpp"

#include "Perlin.hpp"
#include "Simplex.hpp"

namespace np
{
	namespace noise
	{
		/**
		 Turbulence class uses perlin and simplex fractal noise to "turbulate" given x, y, and z
		 then those "turbulated" x, y, and z can be used to get a noise value for any other
		 This class allows access to intern perlin and simplex noise usage
		 */
		class Turbulence : public rng::Random32Base
		{
		private:
			constexpr static chr AsFilename[] = "turbulence.json";

		public:
			constexpr static ui8 PERLIN_DIMENSION_COUNT = 3; // perlin can only support up to 3D
			constexpr static ui8 SIMPLEX_DIMENSION_COUNT = 4; // simplex can only support up to 4D

		private:
			Perlin _perlin[PERLIN_DIMENSION_COUNT];
			Simplex _simplex[SIMPLEX_DIMENSION_COUNT];
			flt _scalar; /// can be anything - default to 0.1f

		public:
			/**
			 constructor
			 */
			Turbulence(const rng::Random32& random_engine = rng::Random32()): rng::Random32Base(random_engine)
			{
				Init();
			}

			/**
			 ddeconstructor
			 */
			~Turbulence() {}

			/**
			 initializes the noise object -- should be done after you call SetRandomEngine
			 sets scalar default value of 0.1
			 */
			inline void Init() override
			{
				rng::Random32Base::Init();

				_scalar = 0.1f;
				rng::Random32 rng;

				for (ui8 i = 0; i < PERLIN_DIMENSION_COUNT; i++)
				{
					rng.SetSeed(GetRandomEngine().CreateSeed());
					_perlin[i].SetRandomEngine(rng);
					_perlin[i].Init();
				}

				for (ui8 i = 0; i < SIMPLEX_DIMENSION_COUNT; i++)
				{
					rng.SetSeed(GetRandomEngine().CreateSeed());
					_simplex[i].SetRandomEngine(rng);
					_simplex[i].Init();
				}
			}

			/**
			 gets the count of dimensions our perlin noise supports
			 */
			inline ui8 GetPerlinCount() const
			{
				return PERLIN_DIMENSION_COUNT;
			}

			/**
			 gets the count of dimensions our simplex noise supports
			 */
			inline ui8 GetSimplexCount() const
			{
				return SIMPLEX_DIMENSION_COUNT;
			}

			/**
			 getter for perlin noise object at given index
			 index range [0, GetPerlinDimensionCount]
			 */
			inline Perlin& GetPerlin(ui8 index)
			{
				return _perlin[index];
			}

			/**
			 getter for perlin noise object at given index
			 index range [0, GetPerlinDimensionCount]
			 */
			inline const Perlin& GetPerlin(ui8 index) const
			{
				return _perlin[index];
			}

			/**
			 getter for simplex noise object at given index
			 index range [0, GetSimplexDimensionCount]
			 */
			inline Simplex& GetSimplex(ui8 index)
			{
				return _simplex[index];
			}

			/**
			 getter for simplex noise object at given index
			 index range [0, GetSimplexDimensionCount]
			 */
			inline const Simplex& GetSimplex(ui8 index) const
			{
				return _simplex[index];
			}

			/**
			 setter for scalar used when adding turbulence
			 */
			inline void SetScalar(flt scalar)
			{
				_scalar = scalar;
			}

			/**
			 getter for scalar used when adding turbulence
			 */
			inline flt GetScalar() const
			{
				return _scalar;
			}

			/**
			 adds perlin noise turbulence to given x
			 */
			inline void TurbulateWithPerlin(flt x) const
			{
				x += _perlin[0].Noise(x, 0, 0) * _scalar;
			}

			/**
			 adds perlin noise turbulence to given xy
			 */
			inline void TurbulateWithPerlin(flt x, flt y) const
			{
				x += _perlin[0].Noise(x, y, 0) * _scalar;
				y += _perlin[1].Noise(x, y, 0) * _scalar;
			}

			/**
			 adds perlin noise turbulence to given xyz
			 */
			inline void TurbulateWithPerlin(flt x, flt y, flt z) const
			{
				x += _perlin[0].Noise(x, y, z) * _scalar;
				y += _perlin[1].Noise(x, y, z) * _scalar;
				z += _perlin[2].Noise(x, y, z) * _scalar;
			}

			/**
			 adds perlin fractal turbulence to given x
			 */
			inline void TurbulateWithPerlinFractal(flt x) const
			{
				x += _perlin[0].Fractal(x, 0, 0) * _scalar;
			}

			/**
			 adds perlin fractal turbulence to given xy
			 */
			inline void TurbulateWithPerlinFractal(flt x, flt y) const
			{
				x += _perlin[0].Fractal(x, y, 0) * _scalar;
				y += _perlin[1].Fractal(x, y, 0) * _scalar;
			}

			/**
			 adds perlin fractal turbulence to given xyz
			 */
			inline void TurbulateWithPerlinFractal(flt x, flt y, flt z) const
			{
				x += _perlin[0].Fractal(x, y, z) * _scalar;
				y += _perlin[1].Fractal(x, y, z) * _scalar;
				z += _perlin[2].Fractal(x, y, z) * _scalar;
			}

			/**
			 adds perlin fractional turbulence to given x
			 */
			inline void TurbulateWithPerlinFractional(flt x) const
			{
				x += _perlin[0].Fractional(x, 0) * _scalar;
			}

			/**
			 adds perlin fractional turbulence to given xy
			 */
			inline void TurbulateWithPerlinFractional(flt x, flt y) const
			{
				x += _perlin[0].Fractional(x, y) * _scalar;
				y += _perlin[1].Fractional(x, y) * _scalar;
			}

			/**
			 adds simplex noise turbulence to given x
			 */
			inline void TurbulateWithSimplex(flt x) const
			{
				x += _simplex[0].Noise(x) * _scalar;
			}

			/**
			 adds simplex noise turbulence to given xy
			 */
			inline void TurbulateWithSimplex(flt x, flt y) const
			{
				x += _simplex[0].Noise(x, y) * _scalar;
				y += _simplex[1].Noise(x, y) * _scalar;
			}

			/**
			 adds simplex noise turbulence to given xyz
			 */
			inline void TurbulateWithSimplex(flt x, flt y, flt z) const
			{
				x += _simplex[0].Noise(x, y, z) * _scalar;
				y += _simplex[1].Noise(x, y, z) * _scalar;
				z += _simplex[2].Noise(x, y, z) * _scalar;
			}

			/**
			 adds simplex noise turbulence to given xyzw
			 */
			inline void TurbulateWithSimplex(flt x, flt y, flt z, flt w) const
			{
				x += _simplex[0].Noise(x, y, z, w) * _scalar;
				y += _simplex[1].Noise(x, y, z, w) * _scalar;
				z += _simplex[2].Noise(x, y, z, w) * _scalar;
				w += _simplex[3].Noise(x, y, z, w) * _scalar;
			}

			/**
			 adds simplex fractal turbulence to given x
			 */
			inline void TurbulateWithSimplexFractal(flt x) const
			{
				x += _simplex[0].Fractal(x) * _scalar;
			}

			/**
			 adds simplex fractal turbulence to given xy
			 */
			inline void TurbulateWithSimplexFractal(flt x, flt y) const
			{
				x += _simplex[0].Fractal(x, y) * _scalar;
				y += _simplex[1].Fractal(x, y) * _scalar;
			}

			/**
			 adds simplex fractal turbulence to given xyz
			 */
			inline void TurbulateWithSimplexFractal(flt x, flt y, flt z) const
			{
				x += _simplex[0].Fractal(x, y, z) * _scalar;
				y += _simplex[1].Fractal(x, y, z) * _scalar;
				z += _simplex[2].Fractal(x, y, z) * _scalar;
			}

			/**
			 adds simplex fractal turbulence to given xyzw
			 */
			inline void TurbulateWithSimplexFractal(flt x, flt y, flt z, flt w) const
			{
				x += _simplex[0].Fractal(x, y, z, w) * _scalar;
				y += _simplex[1].Fractal(x, y, z, w) * _scalar;
				z += _simplex[2].Fractal(x, y, z, w) * _scalar;
				w += _simplex[3].Fractal(x, y, z, w) * _scalar;
			}

			/**
			 adds simplex fractional turbulence to given x
			 */
			inline void TurbulateWithSimplexFractional(flt x) const
			{
				x += _simplex[0].Fractional(x) * _scalar;
			}

			/**
			 adds simplex fractional turbulence to given xy
			 */
			inline void TurbulateWithSimplexFractional(flt x, flt y) const
			{
				x += _simplex[0].Fractional(x, y) * _scalar;
				y += _simplex[1].Fractional(x, y) * _scalar;
			}

			/**
			 adds simplex fractional turbulence to given xyz
			 */
			inline void TurbulateWithSimplexFractional(flt x, flt y, flt z) const
			{
				x += _simplex[0].Fractional(x, y, z) * _scalar;
				y += _simplex[1].Fractional(x, y, z) * _scalar;
				z += _simplex[2].Fractional(x, y, z) * _scalar;
			}
		};
	} // namespace noise
} // namespace np

#endif /* NP_ENGINE_TURBULENCE_HPP */
