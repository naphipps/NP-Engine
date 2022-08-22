//##===----------------------------------------------------------------------===##//
//
//  Author: Nathan Phipps 4/26/20
//
//##===----------------------------------------------------------------------===##//

#ifndef NP_ENGINE_TURBULENCE_HPP
#define NP_ENGINE_TURBULENCE_HPP

#include "NP-Engine/Primitive/Primitive.hpp"
#include "NP-Engine/Random/Random.hpp"
#include "NP-Engine/String/String.hpp"
#include "NP-Engine/FileSystem/FileSystem.hpp"

#include "Perlin.hpp"
#include "Simplex.hpp"

//TODO: can we use containers here for our perline and simplex array??

namespace np::noiz
{
	/*
		Turbulence class uses perlin and simplex fractal noise to "turbulate" given x, y, and z
		then those "turbulated" x, y, and z can be used to get a noise value for any other
		This class allows access to intern perlin and simplex noise usage

		CONSTANTS:

			siz PERLIN_DIMENSION_COUNT = 3
				- perlin can only support up to 3D

			siz SIMPLEX_DIMENSION_COUNT = 4;
				- simplex can only support up to 4D

		MEMBERS:

			Perlin _perlin[PERLIN_DIMENSION_COUNT];

			Simplex _simplex[SIMPLEX_DIMENSION_COUNT];

			flt _scalar;
				- can be anything
				- default to 0.1f
				

		*/

	class Turbulence : public rng::Random32Base
	{
	public:
		constexpr static siz PERLIN_DIMENSION_COUNT = 3;
		constexpr static siz SIMPLEX_DIMENSION_COUNT = 4;

	private:
		Perlin _perlin[PERLIN_DIMENSION_COUNT];
		Simplex _simplex[SIMPLEX_DIMENSION_COUNT];
		flt _scalar;

	public:
		Turbulence(const rng::Random32& random_engine = rng::Random32()): rng::Random32Base(random_engine)
		{
			Init();
		}

		inline void Init() override
		{
			rng::Random32Base::Init();

			_scalar = 0.1f;
			rng::Random32 rng;

			for (siz i = 0; i < PERLIN_DIMENSION_COUNT; i++)
			{
				rng.SetSeed(GetRandomEngine().CreateSeed());
				_perlin[i].SetRandomEngine(rng);
				_perlin[i].Init();
			}

			for (siz i = 0; i < SIMPLEX_DIMENSION_COUNT; i++)
			{
				rng.SetSeed(GetRandomEngine().CreateSeed());
				_simplex[i].SetRandomEngine(rng);
				_simplex[i].Init();
			}
		}

		inline siz GetPerlinCount() const
		{
			return PERLIN_DIMENSION_COUNT;
		}

		inline siz GetSimplexCount() const
		{
			return SIMPLEX_DIMENSION_COUNT;
		}

		inline Perlin& GetPerlin(siz index)
		{
			return _perlin[index];
		}

		inline const Perlin& GetPerlin(siz index) const
		{
			return _perlin[index];
		}

		inline Simplex& GetSimplex(siz index)
		{
			return _simplex[index];
		}

		inline const Simplex& GetSimplex(siz index) const
		{
			return _simplex[index];
		}

		inline void SetScalar(flt scalar)
		{
			_scalar = scalar;
		}

		inline flt GetScalar() const
		{
			return _scalar;
		}

		inline void TurbulateWithPerlin(flt& x) const
		{
			x += _perlin[0].Noise(x, 0, 0) * _scalar;
		}

		inline void TurbulateWithPerlin(flt& x, flt& y) const
		{
			x += _perlin[0].Noise(x, y, 0) * _scalar;
			y += _perlin[1].Noise(x, y, 0) * _scalar;
		}

		inline void TurbulateWithPerlin(flt& x, flt& y, flt& z) const
		{
			x += _perlin[0].Noise(x, y, z) * _scalar;
			y += _perlin[1].Noise(x, y, z) * _scalar;
			z += _perlin[2].Noise(x, y, z) * _scalar;
		}

		inline void TurbulateWithPerlinFractal(flt& x) const
		{
			x += _perlin[0].Fractal(x, 0, 0) * _scalar;
		}

		inline void TurbulateWithPerlinFractal(flt& x, flt& y) const
		{
			x += _perlin[0].Fractal(x, y, 0) * _scalar;
			y += _perlin[1].Fractal(x, y, 0) * _scalar;
		}

		inline void TurbulateWithPerlinFractal(flt& x, flt& y, flt& z) const
		{
			x += _perlin[0].Fractal(x, y, z) * _scalar;
			y += _perlin[1].Fractal(x, y, z) * _scalar;
			z += _perlin[2].Fractal(x, y, z) * _scalar;
		}

		inline void TurbulateWithPerlinFractional(flt& x) const
		{
			x += _perlin[0].Fractional(x, 0) * _scalar;
		}

		inline void TurbulateWithPerlinFractional(flt& x, flt& y) const
		{
			x += _perlin[0].Fractional(x, y) * _scalar;
			y += _perlin[1].Fractional(x, y) * _scalar;
		}

		inline void TurbulateWithSimplex(flt& x) const
		{
			x += _simplex[0].Noise(x) * _scalar;
		}

		inline void TurbulateWithSimplex(flt& x, flt& y) const
		{
			x += _simplex[0].Noise(x, y) * _scalar;
			y += _simplex[1].Noise(x, y) * _scalar;
		}

		inline void TurbulateWithSimplex(flt& x, flt& y, flt& z) const
		{
			x += _simplex[0].Noise(x, y, z) * _scalar;
			y += _simplex[1].Noise(x, y, z) * _scalar;
			z += _simplex[2].Noise(x, y, z) * _scalar;
		}

		inline void TurbulateWithSimplex(flt& x, flt& y, flt& z, flt& w) const
		{
			x += _simplex[0].Noise(x, y, z, w) * _scalar;
			y += _simplex[1].Noise(x, y, z, w) * _scalar;
			z += _simplex[2].Noise(x, y, z, w) * _scalar;
			w += _simplex[3].Noise(x, y, z, w) * _scalar;
		}

		inline void TurbulateWithSimplexFractal(flt& x) const
		{
			x += _simplex[0].Fractal(x) * _scalar;
		}

		inline void TurbulateWithSimplexFractal(flt& x, flt& y) const
		{
			x += _simplex[0].Fractal(x, y) * _scalar;
			y += _simplex[1].Fractal(x, y) * _scalar;
		}

		inline void TurbulateWithSimplexFractal(flt& x, flt& y, flt& z) const
		{
			x += _simplex[0].Fractal(x, y, z) * _scalar;
			y += _simplex[1].Fractal(x, y, z) * _scalar;
			z += _simplex[2].Fractal(x, y, z) * _scalar;
		}

		inline void TurbulateWithSimplexFractal(flt& x, flt& y, flt& z, flt& w) const
		{
			x += _simplex[0].Fractal(x, y, z, w) * _scalar;
			y += _simplex[1].Fractal(x, y, z, w) * _scalar;
			z += _simplex[2].Fractal(x, y, z, w) * _scalar;
			w += _simplex[3].Fractal(x, y, z, w) * _scalar;
		}

		inline void TurbulateWithSimplexFractional(flt& x) const
		{
			x += _simplex[0].Fractional(x) * _scalar;
		}

		inline void TurbulateWithSimplexFractional(flt& x, flt& y) const
		{
			x += _simplex[0].Fractional(x, y) * _scalar;
			y += _simplex[1].Fractional(x, y) * _scalar;
		}

		inline void TurbulateWithSimplexFractional(flt& x, flt& y, flt& z) const
		{
			x += _simplex[0].Fractional(x, y, z) * _scalar;
			y += _simplex[1].Fractional(x, y, z) * _scalar;
			z += _simplex[2].Fractional(x, y, z) * _scalar;
		}
	};
} // namespace np

#endif /* NP_ENGINE_TURBULENCE_HPP */
