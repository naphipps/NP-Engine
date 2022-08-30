//##===----------------------------------------------------------------------===##//
//
//  Author: Nathan Phipps 4/26/20
//
//##===----------------------------------------------------------------------===##//

#ifndef NP_ENGINE_TURBULENCE_HPP
#define NP_ENGINE_TURBULENCE_HPP

#include "NP-Engine/Primitive/Primitive.hpp"
#include "NP-Engine/Container/Container.hpp"
#include "NP-Engine/Random/Random.hpp"

#include "Perlin.hpp"
#include "Simplex.hpp"

namespace np::noiz
{
	/*
		Turbulence class uses perlin and simplex fractal noise to "turbulate" given x, y, and z
		then those "turbulated" x, y, and z can be used to get a noise value for any other
		This class allows access to intern perlin and simplex noise usage

		CONSTANTS:

			flt DEFAULT_SCALAR = 0.1f
				- arbitary value

		MEMBERS:

			con::array<Perlin, 3> _perlins;
				- perlin can only support up to 3D

			con::array<Simplex, 4> _simplexes;
				- simplex can only support up to 4D

			flt _scalar;
				- can be anything
				- default to 0.1f
		*/

	class Turbulence : public rng::Random32Base
	{
	public:
		constexpr static flt DEFAULT_SCALAR = 0.1f;

	private:
		con::array<Perlin, 3> _perlins;
		con::array<Simplex, 4> _simplexes;
		flt _scalar;

	public:
		Turbulence(const rng::Random32& random_engine = rng::Random32()):
			rng::Random32Base(random_engine),
			_scalar(DEFAULT_SCALAR)
		{
			Init();
		}

		inline void Init() override
		{
			rng::Random32Base::Init();

			_scalar = DEFAULT_SCALAR;
			rng::Random32 rng;

			for (siz i = 0; i < _perlins.size(); i++)
			{
				rng.SetSeed(GetRandomEngine().CreateSeed());
				_perlins[i].SetRandomEngine(rng);
				_perlins[i].Init();
			}

			for (siz i = 0; i < _simplexes.size(); i++)
			{
				rng.SetSeed(GetRandomEngine().CreateSeed());
				_simplexes[i].SetRandomEngine(rng);
				_simplexes[i].Init();
			}
		}

		con::array<Perlin, 3>& GetPerlins()
		{
			return _perlins;
		}

		const con::array<Perlin, 3>& GetPerlins() const
		{
			return _perlins;
		}

		con::array<Simplex, 4>& GetSimplexes()
		{
			return _simplexes;
		}

		const con::array<Simplex, 4>& GetSimplexes() const
		{
			return _simplexes;
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
			x += _perlins[0].Noise(x, 0, 0) * _scalar;
		}

		inline void TurbulateWithPerlin(flt& x, flt& y) const
		{
			x += _perlins[0].Noise(x, y, 0) * _scalar;
			y += _perlins[1].Noise(x, y, 0) * _scalar;
		}

		inline void TurbulateWithPerlin(flt& x, flt& y, flt& z) const
		{
			x += _perlins[0].Noise(x, y, z) * _scalar;
			y += _perlins[1].Noise(x, y, z) * _scalar;
			z += _perlins[2].Noise(x, y, z) * _scalar;
		}

		inline void TurbulateWithPerlinFractal(flt& x) const
		{
			x += _perlins[0].Fractal(x, 0, 0) * _scalar;
		}

		inline void TurbulateWithPerlinFractal(flt& x, flt& y) const
		{
			x += _perlins[0].Fractal(x, y, 0) * _scalar;
			y += _perlins[1].Fractal(x, y, 0) * _scalar;
		}

		inline void TurbulateWithPerlinFractal(flt& x, flt& y, flt& z) const
		{
			x += _perlins[0].Fractal(x, y, z) * _scalar;
			y += _perlins[1].Fractal(x, y, z) * _scalar;
			z += _perlins[2].Fractal(x, y, z) * _scalar;
		}

		inline void TurbulateWithPerlinFractional(flt& x) const
		{
			x += _perlins[0].Fractional(x, 0) * _scalar;
		}

		inline void TurbulateWithPerlinFractional(flt& x, flt& y) const
		{
			x += _perlins[0].Fractional(x, y) * _scalar;
			y += _perlins[1].Fractional(x, y) * _scalar;
		}

		inline void TurbulateWithSimplex(flt& x) const
		{
			x += _simplexes[0].Noise(x) * _scalar;
		}

		inline void TurbulateWithSimplex(flt& x, flt& y) const
		{
			x += _simplexes[0].Noise(x, y) * _scalar;
			y += _simplexes[1].Noise(x, y) * _scalar;
		}

		inline void TurbulateWithSimplex(flt& x, flt& y, flt& z) const
		{
			x += _simplexes[0].Noise(x, y, z) * _scalar;
			y += _simplexes[1].Noise(x, y, z) * _scalar;
			z += _simplexes[2].Noise(x, y, z) * _scalar;
		}

		inline void TurbulateWithSimplex(flt& x, flt& y, flt& z, flt& w) const
		{
			x += _simplexes[0].Noise(x, y, z, w) * _scalar;
			y += _simplexes[1].Noise(x, y, z, w) * _scalar;
			z += _simplexes[2].Noise(x, y, z, w) * _scalar;
			w += _simplexes[3].Noise(x, y, z, w) * _scalar;
		}

		inline void TurbulateWithSimplexFractal(flt& x) const
		{
			x += _simplexes[0].Fractal(x) * _scalar;
		}

		inline void TurbulateWithSimplexFractal(flt& x, flt& y) const
		{
			x += _simplexes[0].Fractal(x, y) * _scalar;
			y += _simplexes[1].Fractal(x, y) * _scalar;
		}

		inline void TurbulateWithSimplexFractal(flt& x, flt& y, flt& z) const
		{
			x += _simplexes[0].Fractal(x, y, z) * _scalar;
			y += _simplexes[1].Fractal(x, y, z) * _scalar;
			z += _simplexes[2].Fractal(x, y, z) * _scalar;
		}

		inline void TurbulateWithSimplexFractal(flt& x, flt& y, flt& z, flt& w) const
		{
			x += _simplexes[0].Fractal(x, y, z, w) * _scalar;
			y += _simplexes[1].Fractal(x, y, z, w) * _scalar;
			z += _simplexes[2].Fractal(x, y, z, w) * _scalar;
			w += _simplexes[3].Fractal(x, y, z, w) * _scalar;
		}

		inline void TurbulateWithSimplexFractional(flt& x) const
		{
			x += _simplexes[0].Fractional(x) * _scalar;
		}

		inline void TurbulateWithSimplexFractional(flt& x, flt& y) const
		{
			x += _simplexes[0].Fractional(x, y) * _scalar;
			y += _simplexes[1].Fractional(x, y) * _scalar;
		}

		inline void TurbulateWithSimplexFractional(flt& x, flt& y, flt& z) const
		{
			x += _simplexes[0].Fractional(x, y, z) * _scalar;
			y += _simplexes[1].Fractional(x, y, z) * _scalar;
			z += _simplexes[2].Fractional(x, y, z) * _scalar;
		}
	};
} // namespace np::noiz

#endif /* NP_ENGINE_TURBULENCE_HPP */
