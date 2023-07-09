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
		Turbulence uses noise to "turbulate" given x, y, and z
		then those "turbulated" x, y, and z can be used to get a noise value
	*/
	class Turbulence : public rng::Random64Base
	{
	protected:
		flt _scalar; // can be anything

	public:
		constexpr static flt DEFAULT_SCALAR = 0.1f; // arbitary

		Turbulence(const rng::Random64& engine = rng::Random64()): rng::Random64Base(engine), _scalar(DEFAULT_SCALAR)
		{
			Init();
		}

		inline void Init() override
		{
			rng::Random64Base::Init();
			_scalar = DEFAULT_SCALAR;
		}

		inline void SetScalar(flt scalar)
		{
			_scalar = scalar;
		}

		inline flt GetScalar() const
		{
			return _scalar;
		}
	};

	class PerlinTurbulence : public Turbulence
	{
	protected:
		con::array<Perlin, 3> _perlins;

	public:
		PerlinTurbulence(const rng::Random64& engine = rng::Random64()): Turbulence(engine) {}

		inline void Init() override
		{
			Turbulence::Init();
			for (Perlin& perlin : _perlins)
			{
				perlin.SetRandomEngine(GetRandomEngine().CreateSeed());
				perlin.Init();
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

		inline void Turbulate(flt& x) const
		{
			x += _perlins[0](x, 0, 0) * _scalar;
		}

		inline void Turbulate(flt& x, flt& y) const
		{
			x += _perlins[0](x, y, 0) * _scalar;
			y += _perlins[1](x, y, 0) * _scalar;
		}

		inline void Turbulate(flt& x, flt& y, flt& z) const
		{
			x += _perlins[0](x, y, z) * _scalar;
			y += _perlins[1](x, y, z) * _scalar;
			z += _perlins[2](x, y, z) * _scalar;
		}

		inline void TurbulateFractal(flt& x) const
		{
			x += _perlins[0].Fractal(x, 0, 0) * _scalar;
		}

		inline void TurbulateFractal(flt& x, flt& y) const
		{
			x += _perlins[0].Fractal(x, y, 0) * _scalar;
			y += _perlins[1].Fractal(x, y, 0) * _scalar;
		}

		inline void TurbulateFractal(flt& x, flt& y, flt& z) const
		{
			x += _perlins[0].Fractal(x, y, z) * _scalar;
			y += _perlins[1].Fractal(x, y, z) * _scalar;
			z += _perlins[2].Fractal(x, y, z) * _scalar;
		}

		inline void TurbulateFractional(flt& x) const
		{
			x += _perlins[0].Fractional(x, 0) * _scalar;
		}

		inline void TurbulateFractional(flt& x, flt& y) const
		{
			x += _perlins[0].Fractional(x, y) * _scalar;
			y += _perlins[1].Fractional(x, y) * _scalar;
		}
	};

	class SimplexTurbulence : public Turbulence
	{
	protected:
		con::array<Simplex, 4> _simplexes;

	public:
		SimplexTurbulence(const rng::Random64& engine = rng::Random64()): Turbulence(engine) {}

		inline void Init() override
		{
			Turbulence::Init();
			for (Simplex& simplex : _simplexes)
			{
				simplex.SetRandomEngine(GetRandomEngine().CreateSeed());
				simplex.Init();
			}
		}

		con::array<Simplex, 4>& GetSimplexes()
		{
			return _simplexes;
		}

		const con::array<Simplex, 4>& GetSimplexes() const
		{
			return _simplexes;
		}

		inline void Turbulate(flt& x) const
		{
			x += _simplexes[0](x) * _scalar;
		}

		inline void Turbulate(flt& x, flt& y) const
		{
			x += _simplexes[0](x, y) * _scalar;
			y += _simplexes[1](x, y) * _scalar;
		}

		inline void Turbulate(flt& x, flt& y, flt& z) const
		{
			x += _simplexes[0](x, y, z) * _scalar;
			y += _simplexes[1](x, y, z) * _scalar;
			z += _simplexes[2](x, y, z) * _scalar;
		}

		inline void Turbulate(flt& x, flt& y, flt& z, flt& w) const
		{
			x += _simplexes[0](x, y, z, w) * _scalar;
			y += _simplexes[1](x, y, z, w) * _scalar;
			z += _simplexes[2](x, y, z, w) * _scalar;
			w += _simplexes[3](x, y, z, w) * _scalar;
		}

		inline void TurbulateFractal(flt& x) const
		{
			x += _simplexes[0].Fractal(x) * _scalar;
		}

		inline void TurbulateFractal(flt& x, flt& y) const
		{
			x += _simplexes[0].Fractal(x, y) * _scalar;
			y += _simplexes[1].Fractal(x, y) * _scalar;
		}

		inline void TurbulateFractal(flt& x, flt& y, flt& z) const
		{
			x += _simplexes[0].Fractal(x, y, z) * _scalar;
			y += _simplexes[1].Fractal(x, y, z) * _scalar;
			z += _simplexes[2].Fractal(x, y, z) * _scalar;
		}

		inline void TurbulateFractal(flt& x, flt& y, flt& z, flt& w) const
		{
			x += _simplexes[0].Fractal(x, y, z, w) * _scalar;
			y += _simplexes[1].Fractal(x, y, z, w) * _scalar;
			z += _simplexes[2].Fractal(x, y, z, w) * _scalar;
			w += _simplexes[3].Fractal(x, y, z, w) * _scalar;
		}

		inline void TurbulateFractional(flt& x) const
		{
			x += _simplexes[0].Fractional(x) * _scalar;
		}

		inline void TurbulateFractional(flt& x, flt& y) const
		{
			x += _simplexes[0].Fractional(x, y) * _scalar;
			y += _simplexes[1].Fractional(x, y) * _scalar;
		}

		inline void TurbulateFractional(flt& x, flt& y, flt& z) const
		{
			x += _simplexes[0].Fractional(x, y, z) * _scalar;
			y += _simplexes[1].Fractional(x, y, z) * _scalar;
			z += _simplexes[2].Fractional(x, y, z) * _scalar;
		}
	};
} // namespace np::noiz

#endif /* NP_ENGINE_TURBULENCE_HPP */
