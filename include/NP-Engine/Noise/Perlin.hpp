//##===----------------------------------------------------------------------===##//
//
//  Author: Nathan Phipps ~12/20/18
//
//##===----------------------------------------------------------------------===##//

#ifndef NP_ENGINE_PERLIN_HPP
#define NP_ENGINE_PERLIN_HPP

#include <fstream>

#include "NP-Engine/Primitive/Primitive.hpp"
#include "NP-Engine/Random/Random.hpp"
#include "NP-Engine/Math/Math.hpp"

namespace np::noiz
{
	/*
		improved perlin noise 3D

		MEMBERS:

			ui8 _warp_octave_count;
				- recommend [1, 2]
				- 0 gives you fbm
				- > 3 is not very usable due to performance but produces cool results
				- default to 1 for performance reasons

			flt _warp_octave_multiplier;
				- similar to frequency, as this adds more and more "folds" to the warped shape
				- default to 1
				- recommend [0.5, 1.5] to stay around 1
				- can be anything

			flt _warp_octave_increment;
				- recommend [0, 1] => [smooth, clumpy] but can be whatever
				- this is a very small detail
				- 0.19 was chosen as default

			flt _warp_octave_displacement;
				- recommend [0] for performance reasons
				- can be any value
				- only change if 0 is giving you too "regular" warping
					- we then recommend arbitary [0.19]

			flt _frequency;
				- frequency ("width") of the first octave of noise
				- default to 1.0

			flt _amplitude;
				- amplitude ("height") of the first octave of noise
				- default to 1.0

			flt _lacunarity;
				- lacunarity specifies the frequency multiplier between successive octaves
				- default to 2.0

			flt _persistence;
				- persistence is the loss of amplitude between successive octaves
				- usually 1/lacunarity

			flt _fractional_increment;
				- how much the fractional method increments to prevent fractal artifacts
				- default to 0.19

			ui8 _octave_count;
				- recommends [3, 6] for good control of detail
				- [6, +oo) can be way much detail and perform slow
				- [0, 3] may be too little detail
				- default to 3 for performance reasons

			ui8 _rigidity;
				- higher rigidity means a more stark noise
				- default to 1
				- [0, 8] will use high performance pow
				- [9, 255] will use std::pow

		METHODS:

			Noise
				- return [-amplitude, amplitude]

			CalculateNoise
				- return [-1, 1]

			Fractal
				- the same layer of noise will be added on top of itself an octave count number of times with
					detail respect to lacunarity
				- may contain artifacts due to it layers the same layer of noise on top of itself
				- return [-1, 1]

			Fractional
				- adds an octave count number of layers of noise together with detail respect to lacunarity
				- the added layers are fractional incremenet's value away from each other
					- calculates values in one dimention less than fractal for this reason
				- if fractional increment is zero, then fractional noise may contain artifacts due to it layers
					the same layer of noise on top of itself
				- return [-1, 1]

			RigidFractal
				- same noise as fractal but adds the abs value of each layer then subtracts it
					from 1 then raises it to the rigidity value's power
				- return [0, 1]

			RigidFractional
				- same noise as fractional but adds the abs value of each layer then subtracts
					it from 1 then raises it to the rigidity value's power
				- return [0, 1]

			BillowFractal
				- same noise as rigid fractal but subtracts it from 1

			BillowFractional
				- same noise as rigid fractional but subtracts it from 1

			WarpFractal
				- same noise as fractal but warps it through itself based on warp related fields

			WarpFractional
				- same noise as fractional but warps it through itself based on warp related fields
	*/

	class Perlin : public rng::Random32Base
	{
	public:
		constexpr static flt DEFAULT_FREQUENCY = 1.f;
		constexpr static flt DEFAULT_AMPLITUDE = 1.f;
		constexpr static flt DEFAULT_LACUNARITY = 2.f;
		constexpr static flt DEFAULT_PERSISTENCE = 0.5f;
		constexpr static flt DEFAULT_FRACTIONAL_INCREMENT = 0.19f;
		constexpr static flt DEFAULT_OCTAVE_COUNT = 3;
		constexpr static ui8 DEFAULT_RIGIDITY = 1;
		constexpr static ui8 DEFAULT_WARP_OCTAVE_COUNT = 1;
		constexpr static flt DEFAULT_WARP_OCTAVE_MULTIPLIER = 1.f;
		constexpr static flt DEFAULT_WARP_OCTAVE_INCREMENT = 0.19f;
		constexpr static flt DEFAULT_WARP_OCTAVE_DISPLACEMENT = 0.f;

	private:
		constexpr static ui32 PERMUTATION_SIZE = 512;
		constexpr static ui32 HALF_PERMUTATION_SIZE = 256;
		ui8 _permutation[PERMUTATION_SIZE];
		ui8 _warp_octave_count;
		flt _warp_octave_multiplier;
		flt _warp_octave_increment;
		flt _warp_octave_displacement;
		flt _frequency;
		flt _amplitude;
		flt _lacunarity;
		flt _persistence;
		flt _fractional_increment;
		ui8 _octave_count;
		ui8 _rigidity;

		inline static flt Fade(flt t)
		{
			return t * t * t * (t * (t * 6 - 15) + 10);
		}

		inline static flt Grad(i32 hash, flt x, flt y, flt z)
		{
			i32 h = hash & 15;
			// Convert lower 4 bits of hash into 12 gradient directions
			flt u = h < 8 ? x : y, v = h < 4 ? y : h == 12 || h == 14 ? x : z;
			return ((h & 1) == 0 ? u : -u) + ((h & 2) == 0 ? v : -v);
		}

		inline void InitPermutation()
		{
			for (ui32 i = 0; i < HALF_PERMUTATION_SIZE; i++)
			{
				_permutation[i] = i;
				_permutation[HALF_PERMUTATION_SIZE + i] = i;
			}
		}

	public:
		Perlin(const rng::Random32& engine = rng::Random32()): rng::Random32Base(engine)
		{
			Init();
		}

		inline void Init() override
		{
			rng::Random32Base::Init();

			_frequency = DEFAULT_FREQUENCY;
			_amplitude = DEFAULT_AMPLITUDE;
			_lacunarity = DEFAULT_LACUNARITY;
			_persistence = DEFAULT_PERSISTENCE;
			_fractional_increment = DEFAULT_FRACTIONAL_INCREMENT;
			_octave_count = DEFAULT_OCTAVE_COUNT;
			_rigidity = DEFAULT_RIGIDITY;
			_warp_octave_count = DEFAULT_WARP_OCTAVE_COUNT;
			_warp_octave_multiplier = DEFAULT_WARP_OCTAVE_MULTIPLIER;
			_warp_octave_increment = DEFAULT_WARP_OCTAVE_INCREMENT;
			_warp_octave_displacement = DEFAULT_WARP_OCTAVE_DISPLACEMENT;

			InitPermutation();
			ShufflePermutation();
		}

		inline void ShufflePermutation()
		{
			for (i32 i = 0; i < HALF_PERMUTATION_SIZE; i++)
			{
				i32 index = GetRandomEngine().GetLemireWithinRange(HALF_PERMUTATION_SIZE - i);

				// do not swap with itself
				if (index != PERMUTATION_SIZE - 1 - i)
				{
					// temp = last number in perm yet to be shuffled
					ui8 temp = _permutation[HALF_PERMUTATION_SIZE - 1 - i];
					_permutation[HALF_PERMUTATION_SIZE - 1 - i] = _permutation[index];
					// copy to latter half of permutation
					_permutation[PERMUTATION_SIZE - 1 - i] = _permutation[index];
					_permutation[index] = temp;
				}
			}
		}

		inline void SetFrequency(flt f)
		{
			_frequency = f;
		}

		inline flt GetFrequency() const
		{
			return _frequency;
		}

		inline void SetAmplitude(flt a)
		{
			_amplitude = a;
		}

		inline flt GetAmplitude() const
		{
			return _amplitude;
		}

		inline void SetLacunarity(flt l)
		{
			_lacunarity = l;
		}

		inline flt GetLacunarity() const
		{
			return _lacunarity;
		}

		inline void SetPersistence(flt p)
		{
			_persistence = p;
		}

		inline flt GetPersistence() const
		{
			return _persistence;
		}

		inline void SetFractionalIncrement(flt f)
		{
			_fractional_increment = f;
		}

		inline flt GetFractionalIncrement() const
		{
			return _fractional_increment;
		}

		inline void SetOctaveCount(ui8 o)
		{
			_octave_count = o;
		}

		inline ui8 GetOctaveCount() const
		{
			return _octave_count;
		}

		inline void SetRigidity(ui8 r)
		{
			_rigidity = r;
		}

		inline ui8 GetRigidity() const
		{
			return _rigidity;
		}

		inline void SetWarpOctaveCount(ui8 count)
		{
			_warp_octave_count = count;
		}

		inline ui8 GetWarpOctaveCount() const
		{
			return _warp_octave_count;
		}

		inline void SetWarpOctaveMultiplier(flt multiplier)
		{
			_warp_octave_multiplier = multiplier;
		}

		inline flt GetWarpOctaveMultiplier() const
		{
			return _warp_octave_multiplier;
		}

		inline void SetWarpOctaveIncrement(flt increment)
		{
			_warp_octave_increment = increment;
		}

		inline flt GetWarpOctaveIncrement() const
		{
			return _warp_octave_increment;
		}

		inline void SetWarpOctaveDisplacement(flt displacement)
		{
			_warp_octave_displacement = displacement;
		}

		inline flt GetWarpOctaveDisplacement() const
		{
			return _warp_octave_displacement;
		}

		inline flt Noise(flt x, flt y, flt z) const
		{
			return GetAmplitude() * CalculateNoise(GetFrequency() * x, GetFrequency() * y, GetFrequency() * z);
		}

		inline flt CalculateNoise(flt x, flt y, flt z) const
		{
			// Find the unit cube that contains the point
			i32 X = mat::FastFloor(x);
			i32 Y = mat::FastFloor(y);
			i32 Z = mat::FastFloor(z);

			// Find relative x, y, z of point in cube
			x -= X;
			y -= Y;
			z -= Z;

			X &= 255;
			Y &= 255;
			Z &= 255;

			// Compute fade curves for each of x, y, z
			flt u = Fade(x);
			flt v = Fade(y);
			flt w = Fade(z);

			// Hash coordinates of the 8 cube corners
			i32 A = _permutation[X] + Y;
			i32 AA = _permutation[A] + Z;
			i32 AB = _permutation[A + 1] + Z;
			i32 B = _permutation[X + 1] + Y;
			i32 BA = _permutation[B] + Z;
			i32 BB = _permutation[B + 1] + Z;

			// Add blended results from 8 corners of cube
			flt res = ::glm::lerp(
				::glm::lerp(::glm::lerp(Grad(_permutation[AA], x, y, z), Grad(_permutation[BA], x - 1, y, z), u),
							::glm::lerp(Grad(_permutation[AB], x, y - 1, z), Grad(_permutation[BB], x - 1, y - 1, z), u), v),
				::glm::lerp(
					::glm::lerp(Grad(_permutation[AA + 1], x, y, z - 1), Grad(_permutation[BA + 1], x - 1, y, z - 1), u),
					::glm::lerp(Grad(_permutation[AB + 1], x, y - 1, z - 1), Grad(_permutation[BB + 1], x - 1, y - 1, z - 1),
								u),
					v),
				w);

			return res;
		}

		inline flt Fractal(flt x, flt y, flt z) const
		{
			flt output = 0.f;
			flt denom = 0.f;
			flt frequency = _frequency;
			flt amplitude = _amplitude;

			for (ui8 i = 0; i < _octave_count; i++)
			{
				output += amplitude * CalculateNoise(x * frequency, y * frequency, z * frequency);
				denom += amplitude;
				frequency *= _lacunarity;
				amplitude *= _persistence;
			}

			return output / denom;
		}

		inline flt Fractional(flt x, flt y) const
		{
			flt output = 0.f;
			flt denom = 0.f;
			flt frequency = _frequency;
			flt amplitude = _amplitude;
			flt fractionalIncrement = _fractional_increment;

			for (ui8 i = 0; i < _octave_count; i++)
			{
				output += amplitude * CalculateNoise(x * frequency, y * frequency, i * fractionalIncrement * frequency);
				denom += amplitude;
				frequency *= _lacunarity;
				amplitude *= _persistence;
			}

			return output / denom;
		}

		inline flt RigidFractal(flt x, flt y, flt z) const
		{
			flt output = 0.f;
			flt denom = 0.f;
			flt frequency = _frequency;
			flt amplitude = _amplitude;

			for (ui8 i = 0; i < _octave_count; i++)
			{
				output += amplitude * ::std::abs(CalculateNoise(x * frequency, y * frequency, z * frequency));
				denom += amplitude;
				frequency *= _lacunarity;
				amplitude *= _persistence;
			}

			output /= denom;
			output = mat::Flip(output);

			switch (_rigidity)
			{
			case 0:
				output = mat::Pow0(output);
				break;
			case 1:
				output = mat::Pow1(output);
				break;
			case 2:
				output = mat::Pow2(output);
				break;
			case 3:
				output = mat::Pow3(output);
				break;
			case 4:
				output = mat::Pow4(output);
				break;
			case 5:
				output = mat::Pow5(output);
				break;
			case 6:
				output = mat::Pow6(output);
				break;
			case 7:
				output = mat::Pow7(output);
				break;
			case 8:
				output = mat::Pow8(output);
				break;
			default:
				output = ::std::pow(output, _rigidity);
				break;
			}

			return output;
		}

		inline flt RigidFractional(flt x, flt y) const
		{
			flt output = 0.f;
			flt denom = 0.f;
			flt frequency = _frequency;
			flt amplitude = _amplitude;

			for (ui8 i = 0; i < _octave_count; i++)
			{
				output +=
					amplitude * ::std::abs(CalculateNoise(x * frequency, y * frequency, i * _fractional_increment * frequency));
				denom += amplitude;
				frequency *= _lacunarity;
				amplitude *= _persistence;
			}

			output /= denom;
			output = mat::Flip(output);

			switch (_rigidity)
			{
			case 0:
				output = mat::Pow0(output);
				break;
			case 1:
				output = mat::Pow1(output);
				break;
			case 2:
				output = mat::Pow2(output);
				break;
			case 3:
				output = mat::Pow3(output);
				break;
			case 4:
				output = mat::Pow4(output);
				break;
			case 5:
				output = mat::Pow5(output);
				break;
			case 6:
				output = mat::Pow6(output);
				break;
			case 7:
				output = mat::Pow7(output);
				break;
			case 8:
				output = mat::Pow8(output);
				break;
			default:
				output = ::std::pow(output, _rigidity);
				break;
			}

			return output;
		}

		inline flt BillowFractal(flt x, flt y, flt z) const
		{
			return mat::Flip(RigidFractal(x, y, z));
		}

		inline flt BillowFractional(flt x, flt y) const
		{
			return mat::Flip(RigidFractional(x, y));
		}

		inline flt WarpFractal(flt x, flt y, flt z) const
		{
			ui8 woc = _warp_octave_count;
			flt wom = _warp_octave_multiplier;
			flt woi = _warp_octave_increment;
			flt wod = _warp_octave_displacement;

			flt wx = 0; // warping x
			flt wy = 0; // warping y
			flt wz = 0; // warping z
			flt output = 0;

			if (wod == 0)
			{
				// with wod = 0, ox and oy would end up being the same value, so we just use ox below
				for (ui8 i = 0; i < woc; i++)
					wx = Fractal(x + wom * wx + i * woi, y + wom * wx + i * woi, z + wom * wz + i * woi);

				output = Fractal(x + wom * wx + woc * woi, y + wom * wx + woc * woi, z + wom * wz + woc * woi);
			}
			else
			{
				for (ui8 i = 0; i < woc; i++)
				{
					wx = Fractal(x + wom * wx + i * woi, y + wom * wy + i * woi, z + wom * wz + i * woi);
					wy =
						Fractal(x + wom * wx + (i + wod) * woi, y + wom * wy + (i + wod) * woi, z + wom * wz + (i + wod) * woi);
					wz = Fractal(x + wom * wx + (i + 2 * wod) * woi, y + wom * wy + (i + 2 * wod) * woi,
								 z + wom * wz + (i + 2 * wod) * woi);
				}

				output = Fractal(x + wom * wx + woc * woi, y + wom * wy + woc * woi, z + wom * wz + woc * woi);
			}

			return output;
		}

		inline flt WarpFractional(flt x, flt y) const
		{
			ui8 woc = _warp_octave_count;
			flt wom = _warp_octave_multiplier;
			flt woi = _warp_octave_increment;
			flt wod = _warp_octave_displacement;

			flt wx = 0; // warping x
			flt wy = 0; // warping y
			flt output = 0;

			if (wod == 0)
			{
				// with wod = 0, ox and oy would end up being the same value, so we just use ox below
				for (ui8 i = 0; i < woc; i++)
					wx = Fractional(x + wom * wx + i * woi, y + wom * wx + i * woi);

				output = Fractional(x + wom * wx + woc * woi, y + wom * wx + woc * woi);
			}
			else
			{
				for (ui8 i = 0; i < woc; i++)
				{
					wx = Fractional(x + wom * wx + i * woi, y + wom * wy + i * woi);
					wy = Fractional(x + wom * wx + (i + wod) * woi, y + wom * wy + (i + wod) * woi);
				}

				output = Fractional(x + wom * wx + woc * woi, y + wom * wy + woc * woi);
			}

			return output;
		}
	};
} // namespace np::noiz

#endif /* NP_ENGINE_PERLIN_HPP */
