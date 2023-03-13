//##===----------------------------------------------------------------------===##//
//
//  Author: Nathan Phipps ~12/20/18
//
//##===----------------------------------------------------------------------===##//

#ifndef NP_ENGINE_SIMPLEX_HPP
#define NP_ENGINE_SIMPLEX_HPP

#include <fstream>

#include "NP-Engine/Primitive/Primitive.hpp"
#include "NP-Engine/Random/Random.hpp"
#include "NP-Engine/Math/Math.hpp"

// TODO: use Random64
// TODO: add SIMD

namespace np::noiz
{
	/*
		simplex noise 1D, 2D, 3D, 4D

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

			PureNoise
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

	class Simplex : public rng::Random32Base
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
		constexpr static siz GRADIENT_4D_SIZE = 128;
		constexpr static flt GRADIENT_4D[GRADIENT_4D_SIZE] = {
			0.f,  1.f,	1.f,  1.f,	0.f,  1.f,	1.f,  -1.f, 0.f,  1.f,	-1.f, 1.f,	0.f,  1.f,	-1.f, -1.f, 0.f,  -1.f, 1.f,
			1.f,  0.f,	-1.f, 1.f,	-1.f, 0.f,	-1.f, -1.f, 1.f,  0.f,	-1.f, -1.f, -1.f, 1.f,	0.f,  1.f,	1.f,  1.f,	0.f,
			1.f,  -1.f, 1.f,  0.f,	-1.f, 1.f,	1.f,  0.f,	-1.f, -1.f, -1.f, 0.f,	1.f,  1.f,	-1.f, 0.f,	1.f,  -1.f, -1.f,
			0.f,  -1.f, 1.f,  -1.f, 0.f,  -1.f, -1.f, 1.f,	1.f,  0.f,	1.f,  1.f,	1.f,  0.f,	-1.f, 1.f,	-1.f, 0.f,	1.f,
			1.f,  -1.f, 0.f,  -1.f, -1.f, 1.f,	0.f,  1.f,	-1.f, 1.f,	0.f,  -1.f, -1.f, -1.f, 0.f,  1.f,	-1.f, -1.f, 0.f,
			-1.f, 1.f,	1.f,  1.f,	0.f,  1.f,	1.f,  -1.f, 0.f,  1.f,	-1.f, 1.f,	0.f,  1.f,	-1.f, -1.f, 0.f,  -1.f, 1.f,
			1.f,  0.f,	-1.f, 1.f,	-1.f, 0.f,	-1.f, -1.f, 1.f,  0.f,	-1.f, -1.f, -1.f, 0.f};

	private:
		static const siz PERMUTATION_SIZE = 256;
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

		inline ui8 Hash(i32 i) const
		{
			return _permutation[static_cast<ui8>(i)];
		}

		inline static flt Grad(i32 hash, flt x)
		{
			i32 h = hash & 0x0F; // Convert low 4 bits of hash code
			flt grad = 1.0f + (h & 7); // Gradient value 1.0, 2.0, ..., 8.0
			if ((h & 8) != 0)
				grad = -grad; // Set a random sign for the gradient
			return (grad * x); // Multiply the gradient with the distance
		}

		inline static flt Grad(i32 hash, flt x, flt y)
		{
			i32 h = hash & 0x3F; // Convert low 3 bits of hash code
			flt u = h < 4 ? x : y; // into 8 simple gradient directions,
			flt v = h < 4 ? y : x;
			return ((h & 1) ? -u : u) + ((h & 2) ? -2.0f * v : 2.0f * v); // and compute the dot product with (x,y).
		}

		inline static flt Grad(i32 hash, flt x, flt y, flt z)
		{
			i32 h = hash & 15; // Convert low 4 bits of hash code into 12 simple
			flt u = h < 8 ? x : y; // gradient directions, and compute dot product.
			flt v = h < 4 ? y : h == 12 || h == 14 ? x : z; // Fix repeats at h = 12 to 15
			return ((h & 1) ? -u : u) + ((h & 2) ? -v : v);
		}

		inline static flt Grad(i32 hash, flt x, flt y, flt z, flt w)
		{
			i32 i = (hash & 31) << 2;
			return x * GRADIENT_4D[i] + y * GRADIENT_4D[i + 1] + z * GRADIENT_4D[i + 2] + w * GRADIENT_4D[i + 3];
		}

		inline void InitPermutation()
		{
			for (siz i = 0; i < PERMUTATION_SIZE; i++)
				_permutation[i] = i;
		}

	public:
		Simplex(const rng::Random32& random_engine = rng::Random32()): rng::Random32Base(random_engine)
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
			siz last_index = PERMUTATION_SIZE - 1;
			for (siz i = 0; i < PERMUTATION_SIZE; i++)
			{
				siz index = last_index - i;
				siz swap = GetRandomEngine().GetLemireWithinRange(PERMUTATION_SIZE);

				// optimization - no need to swap with self
				if (index != swap)
				{
					ui8 temp = _permutation[index];
					_permutation[index] = _permutation[swap];
					_permutation[swap] = temp;
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

		inline flt Noise(flt x) const // TODO: this could be an operator()
		{
			return GetAmplitude() * PureNoise(GetFrequency() * x);
		}

		inline flt Noise(flt x, flt y) const // TODO: this could be an operator()
		{
			return GetAmplitude() * PureNoise(GetFrequency() * x, GetFrequency() * y);
		}

		inline flt Noise(flt x, flt y, flt z) const // TODO: this could be an operator()
		{
			return GetAmplitude() * PureNoise(GetFrequency() * x, GetFrequency() * y, GetFrequency() * z);
		}

		inline flt Noise(flt x, flt y, flt z, flt w) const // TODO: this could be an operator()
		{
			return GetAmplitude() * PureNoise(GetFrequency() * x, GetFrequency() * y, GetFrequency() * z, GetFrequency() * w);
		}

		/*
			calculates noise value based directly on params and not affected by internal properties
		*/
		inline flt PureNoise(flt x) const // TODO: consider refactoring to CalculateNoiseValue
		{
			flt n0, n1; // Noise contributions from the two "corners"

			// No need to skew the input space in 1D

			// Corners coordinates (nearest integer values):
			i32 i0 = mat::FastFloor(x);
			i32 i1 = i0 + 1;
			// Distances to corners (between 0 and 1):
			flt x0 = x - i0;
			flt x1 = x0 - 1.0f;

			// Calculate the contribution from the first corner
			flt t0 = 1.0f - x0 * x0;
			//  if(t0 < 0.0f) t0 = 0.0f; // not possible
			t0 *= t0;
			n0 = t0 * t0 * Grad(Hash(i0), x0);

			// Calculate the contribution from the second corner
			flt t1 = 1.0f - x1 * x1;
			//  if(t1 < 0.0f) t1 = 0.0f; // not possible
			t1 *= t1;
			n1 = t1 * t1 * Grad(Hash(i1), x1);

			// The maximum value of this noise is 8*(3/4)^4 = 2.53125
			// A factor of 0.395 scales to fit exactly within [-1,1]
			return 0.395f * (n0 + n1);
		}

		/*
			calculates noise value based directly on params and not affected by internal properties
		*/
		inline flt PureNoise(flt x, flt y) const // TODO: consider refactoring to CalculateNoiseValue
		{
			flt n0, n1, n2; // Noise contributions from the three corners

			// Skewing/Unskewing factors for 2D
			static const flt F2 = 0.366025403f; // F2 = (::std::sqrt(3) - 1) / 2
			static const flt G2 = 0.211324865f; // G2 = (3 - ::std::sqrt(3)) / 6   = F2 / (1 + 2 * K) //TODO: what is K?

			// Skew the input space to determine which simplex cell we're in
			flt s = (x + y) * F2; // Hairy factor for 2D
			flt xs = x + s;
			flt ys = y + s;
			i32 i = mat::FastFloor(xs);
			i32 j = mat::FastFloor(ys);

			// Unskew the cell origin back to (x,y) space
			flt t = static_cast<flt>(i + j) * G2;
			flt X0 = i - t;
			flt Y0 = j - t;
			flt x0 = x - X0; // The x,y distances from the cell origin
			flt y0 = y - Y0;

			// For the 2D case, the simplex shape is an equilateral triangle.
			// Determine which simplex we are in.
			i32 i1, j1; // Offsets for second (middle) corner of simplex in (i,j) coords
			if (x0 > y0) // lower triangle, XY order: (0,0)->(1,0)->(1,1)
			{
				i1 = 1;
				j1 = 0;
			}
			else // upper triangle, YX order: (0,0)->(0,1)->(1,1)
			{
				i1 = 0;
				j1 = 1;
			}

			// A step of (1,0) in (i,j) means a step of (1-c,-c) in (x,y), and
			// a step of (0,1) in (i,j) means a step of (-c,1-c) in (x,y), where
			// c = (3-::std::sqrt(3))/6

			flt x1 = x0 - i1 + G2; // Offsets for middle corner in (x,y) unskewed coords
			flt y1 = y0 - j1 + G2;
			flt x2 = x0 - 1.0f + 2.0f * G2; // Offsets for last corner in (x,y) unskewed coords
			flt y2 = y0 - 1.0f + 2.0f * G2;

			// Calculate the contribution from the first corner
			flt t0 = 0.5f - x0 * x0 - y0 * y0;
			if (t0 < 0.0f)
			{
				n0 = 0.0f;
			}
			else
			{
				// Work out the hashed gradient indices of the three simplex corners
				i32 gi0 = Hash(i + Hash(j));
				t0 *= t0;
				n0 = t0 * t0 * Grad(gi0, x0, y0);
			}

			// Calculate the contribution from the second corner
			flt t1 = 0.5f - x1 * x1 - y1 * y1;
			if (t1 < 0.0f)
			{
				n1 = 0.0f;
			}
			else
			{
				// Work out the hashed gradient indices of the three simplex corners
				i32 gi1 = Hash(i + i1 + Hash(j + j1));
				t1 *= t1;
				n1 = t1 * t1 * Grad(gi1, x1, y1);
			}

			// Calculate the contribution from the third corner
			flt t2 = 0.5f - x2 * x2 - y2 * y2;
			if (t2 < 0.0f)
			{
				n2 = 0.0f;
			}
			else
			{
				// Work out the hashed gradient indices of the three simplex corners
				i32 gi2 = Hash(i + 1 + Hash(j + 1));
				t2 *= t2;
				n2 = t2 * t2 * Grad(gi2, x2, y2);
			}

			// Add contributions from each corner to get the final noise value.
			// The result is scaled to return values in the interval [-1,1].
			return 45.23065f * (n0 + n1 + n2);
		}

		/*
			calculates noise value based directly on params and not affected by internal properties
		*/
		inline flt PureNoise(flt x, flt y, flt z) const // TODO: consider refactoring to CalculateNoiseValue
		{
			flt n0, n1, n2, n3; // Noise contributions from the four corners

			// Skewing/Unskewing factors for 3D
			static const flt F3 = 1.0f / 3.0f; // TODO: consider writing out the result
			static const flt G3 = 1.0f / 6.0f; // TODO: consider writing out the result

			// Skew the input space to determine which simplex cell we're in
			flt s = (x + y + z) * F3; // Very nice and simple skew factor for 3D
			i32 i = mat::FastFloor(x + s);
			i32 j = mat::FastFloor(y + s);
			i32 k = mat::FastFloor(z + s);
			flt t = (i + j + k) * G3;
			flt X0 = i - t; // Unskew the cell origin back to (x,y,z) space
			flt Y0 = j - t;
			flt Z0 = k - t;
			flt x0 = x - X0; // The x,y,z distances from the cell origin
			flt y0 = y - Y0;
			flt z0 = z - Z0;

			// For the 3D case, the simplex shape is a slightly irregular tetrahedron.
			// Determine which simplex we are in.
			i32 i1, j1, k1; // Offsets for second corner of simplex in (i,j,k) coords
			i32 i2, j2, k2; // Offsets for third corner of simplex in (i,j,k) coords
			if (x0 >= y0)
			{
				if (y0 >= z0)
				{
					i1 = 1;
					j1 = 0;
					k1 = 0;
					i2 = 1;
					j2 = 1;
					k2 = 0; // X Y Z order
				}
				else if (x0 >= z0)
				{
					i1 = 1;
					j1 = 0;
					k1 = 0;
					i2 = 1;
					j2 = 0;
					k2 = 1; // X Z Y order
				}
				else
				{
					i1 = 0;
					j1 = 0;
					k1 = 1;
					i2 = 1;
					j2 = 0;
					k2 = 1; // Z X Y order
				}
			}
			else // x0<y0
			{
				if (y0 < z0)
				{
					i1 = 0;
					j1 = 0;
					k1 = 1;
					i2 = 0;
					j2 = 1;
					k2 = 1; // Z Y X order
				}
				else if (x0 < z0)
				{
					i1 = 0;
					j1 = 1;
					k1 = 0;
					i2 = 0;
					j2 = 1;
					k2 = 1; // Y Z X order
				}
				else
				{
					i1 = 0;
					j1 = 1;
					k1 = 0;
					i2 = 1;
					j2 = 1;
					k2 = 0; // Y X Z order
				}
			}

			// A step of (1,0,0) in (i,j,k) means a step of (1-c,-c,-c) in (x,y,z),
			// a step of (0,1,0) in (i,j,k) means a step of (-c,1-c,-c) in (x,y,z), and
			// a step of (0,0,1) in (i,j,k) means a step of (-c,-c,1-c) in (x,y,z), where
			// c = 1/6.
			flt x1 = x0 - i1 + G3; // Offsets for second corner in (x,y,z) coords
			flt y1 = y0 - j1 + G3;
			flt z1 = z0 - k1 + G3;
			flt x2 = x0 - i2 + 2.0f * G3; // Offsets for third corner in (x,y,z) coords
			flt y2 = y0 - j2 + 2.0f * G3;
			flt z2 = z0 - k2 + 2.0f * G3;
			flt x3 = x0 - 1.0f + 3.0f * G3; // Offsets for last corner in (x,y,z) coords
			flt y3 = y0 - 1.0f + 3.0f * G3;
			flt z3 = z0 - 1.0f + 3.0f * G3;

			// Calculate the contribution from the four corners
			flt t0 = 0.6f - x0 * x0 - y0 * y0 - z0 * z0;
			if (t0 < 0)
			{
				n0 = 0.0;
			}
			else
			{
				// Work out the hashed gradient indices of the four simplex corners
				i32 gi0 = Hash(i + Hash(j + Hash(k)));
				t0 *= t0;
				n0 = t0 * t0 * Grad(gi0, x0, y0, z0);
			}

			flt t1 = 0.6f - x1 * x1 - y1 * y1 - z1 * z1;
			if (t1 < 0)
			{
				n1 = 0.0;
			}
			else
			{
				// Work out the hashed gradient indices of the four simplex corners
				i32 gi1 = Hash(i + i1 + Hash(j + j1 + Hash(k + k1)));
				t1 *= t1;
				n1 = t1 * t1 * Grad(gi1, x1, y1, z1);
			}

			flt t2 = 0.6f - x2 * x2 - y2 * y2 - z2 * z2;
			if (t2 < 0)
			{
				n2 = 0.0;
			}
			else
			{
				// Work out the hashed gradient indices of the four simplex corners
				i32 gi2 = Hash(i + i2 + Hash(j + j2 + Hash(k + k2)));
				t2 *= t2;
				n2 = t2 * t2 * Grad(gi2, x2, y2, z2);
			}

			flt t3 = 0.6f - x3 * x3 - y3 * y3 - z3 * z3;
			if (t3 < 0)
			{
				n3 = 0.0;
			}
			else
			{
				// Work out the hashed gradient indices of the four simplex corners
				i32 gi3 = Hash(i + 1 + Hash(j + 1 + Hash(k + 1)));
				t3 *= t3;
				n3 = t3 * t3 * Grad(gi3, x3, y3, z3);
			}
			// Add contributions from each corner to get the final noise value.
			// The result is scaled to stay just inside [-1,1]
			return 32.0f * (n0 + n1 + n2 + n3);
		}

		/*
			calculates noise value based directly on params and not affected by internal properties
		*/
		inline flt PureNoise(flt x, flt y, flt z, flt w) const // TODO: consider refactoring to CalculateNoiseValue
		{
			flt n0, n1, n2, n3, n4;

			// Skewing/Unskewing factors for 4D
			static const flt F4 = (flt)(::std::sqrt(5.f) - 1.f) / 4.f; // TODO: consider writing out the result
			static const flt G4 = (flt)(5.f - ::std::sqrt(5.f)) / 20.f; // TODO: consider writing out the result

			flt t = (x + y + z + w) * F4;
			i32 i = mat::FastFloor(x + t);
			i32 j = mat::FastFloor(y + t);
			i32 k = mat::FastFloor(z + t);
			i32 l = mat::FastFloor(w + t);
			t = (i + j + k + l) * G4;
			flt X0 = i - t;
			flt Y0 = j - t;
			flt Z0 = k - t;
			flt W0 = l - t;
			flt x0 = x - X0;
			flt y0 = y - Y0;
			flt z0 = z - Z0;
			flt w0 = w - W0;

			i32 rankx = 0;
			i32 ranky = 0;
			i32 rankz = 0;
			i32 rankw = 0;

			if (x0 > y0)
				rankx++;
			else
				ranky++;

			if (x0 > z0)
				rankx++;
			else
				rankz++;

			if (x0 > w0)
				rankx++;
			else
				rankw++;

			if (y0 > z0)
				ranky++;
			else
				rankz++;

			if (y0 > w0)
				ranky++;
			else
				rankw++;

			if (z0 > w0)
				rankz++;
			else
				rankw++;

			i32 i1 = rankx >= 3 ? 1 : 0;
			i32 j1 = ranky >= 3 ? 1 : 0;
			i32 k1 = rankz >= 3 ? 1 : 0;
			i32 l1 = rankw >= 3 ? 1 : 0;

			i32 i2 = rankx >= 2 ? 1 : 0;
			i32 j2 = ranky >= 2 ? 1 : 0;
			i32 k2 = rankz >= 2 ? 1 : 0;
			i32 l2 = rankw >= 2 ? 1 : 0;

			i32 i3 = rankx >= 1 ? 1 : 0;
			i32 j3 = ranky >= 1 ? 1 : 0;
			i32 k3 = rankz >= 1 ? 1 : 0;
			i32 l3 = rankw >= 1 ? 1 : 0;

			flt x1 = x0 - i1 + G4;
			flt y1 = y0 - j1 + G4;
			flt z1 = z0 - k1 + G4;
			flt w1 = w0 - l1 + G4;
			flt x2 = x0 - i2 + 2 * G4;
			flt y2 = y0 - j2 + 2 * G4;
			flt z2 = z0 - k2 + 2 * G4;
			flt w2 = w0 - l2 + 2 * G4;
			flt x3 = x0 - i3 + 3 * G4;
			flt y3 = y0 - j3 + 3 * G4;
			flt z3 = z0 - k3 + 3 * G4;
			flt w3 = w0 - l3 + 3 * G4;
			flt x4 = x0 - 1 + 4 * G4;
			flt y4 = y0 - 1 + 4 * G4;
			flt z4 = z0 - 1 + 4 * G4;
			flt w4 = w0 - 1 + 4 * G4;

			t = flt(0.6) - x0 * x0 - y0 * y0 - z0 * z0 - w0 * w0;
			if (t < 0)
			{
				n0 = 0;
			}
			else
			{
				i32 gi0 = Hash(i + Hash(j + Hash(k + Hash(l))));
				t *= t;
				n0 = t * t * Grad(gi0, x0, y0, z0, w0);
			}

			t = flt(0.6) - x1 * x1 - y1 * y1 - z1 * z1 - w1 * w1;
			if (t < 0)
			{
				n1 = 0;
			}
			else
			{
				i32 gi1 = Hash(i + i1 + Hash(j + j1 + Hash(k + k1 + Hash(l + l1))));
				t *= t;
				n1 = t * t * Grad(gi1, x1, y1, z1, w1);
			}

			t = flt(0.6) - x2 * x2 - y2 * y2 - z2 * z2 - w2 * w2;
			if (t < 0)
			{
				n2 = 0;
			}
			else
			{
				i32 gi2 = Hash(i + i2 + Hash(j + j2 + Hash(k + k2 + Hash(l + l2))));
				t *= t;
				n2 = t * t * Grad(gi2, x2, y2, z2, w2);
			}

			t = flt(0.6) - x3 * x3 - y3 * y3 - z3 * z3 - w3 * w3;
			if (t < 0)
			{
				n3 = 0;
			}
			else
			{
				i32 gi3 = Hash(i + i3 + Hash(j + j3 + Hash(k + k3 + Hash(l + l3))));
				t *= t;
				n3 = t * t * Grad(gi3, x3, y3, z3, w3);
			}

			t = flt(0.6) - x4 * x4 - y4 * y4 - z4 * z4 - w4 * w4;
			if (t < 0)
			{
				n4 = 0;
			}
			else
			{
				i32 gi4 = Hash(i + 1 + Hash(j + 1 + Hash(k + 1 + Hash(l + 1))));
				t *= t;
				n4 = t * t * Grad(gi4, x4, y4, z4, w4);
			}

			return 27 * (n0 + n1 + n2 + n3 + n4);
		}

		inline flt Fractal(flt x) const
		{
			flt output = 0.f;
			flt denom = 0.f;
			flt frequency = _frequency;
			flt amplitude = _amplitude;

			for (ui8 i = 0; i < _octave_count; i++)
			{
				output += amplitude * PureNoise(x * frequency);
				denom += amplitude;
				frequency *= _lacunarity;
				amplitude *= _persistence;
			}

			return output / denom;
		}

		inline flt Fractal(flt x, flt y) const
		{
			flt output = 0.f;
			flt denom = 0.f;
			flt frequency = _frequency;
			flt amplitude = _amplitude;

			for (ui8 i = 0; i < _octave_count; i++)
			{
				output += amplitude * PureNoise(x * frequency, y * frequency);
				denom += amplitude;
				frequency *= _lacunarity;
				amplitude *= _persistence;
			}

			return output / denom;
		}

		inline flt Fractal(flt x, flt y, flt z) const
		{
			flt output = 0.f;
			flt denom = 0.f;
			flt frequency = _frequency;
			flt amplitude = _amplitude;

			for (ui8 i = 0; i < _octave_count; i++)
			{
				output += amplitude * PureNoise(x * frequency, y * frequency, z * frequency);
				denom += amplitude;
				frequency *= _lacunarity;
				amplitude *= _persistence;
			}

			return output / denom;
		}

		inline flt Fractal(flt x, flt y, flt z, flt w) const
		{
			flt output = 0.f;
			flt denom = 0.f;
			flt frequency = _frequency;
			flt amplitude = _amplitude;

			for (ui8 i = 0; i < _octave_count; i++)
			{
				output += amplitude * PureNoise(x * frequency, y * frequency, z * frequency, w * frequency);
				denom += amplitude;
				frequency *= _lacunarity;
				amplitude *= _persistence;
			}

			return output / denom;
		}

		inline flt Fractional(flt x) const
		{
			flt output = 0.f;
			flt denom = 0.f;
			flt frequency = _frequency;
			flt amplitude = _amplitude;

			for (ui8 i = 0; i < _octave_count; i++)
			{
				output += amplitude * PureNoise(x * frequency, i * _fractional_increment * frequency);
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

			for (ui8 i = 0; i < _octave_count; i++)
			{
				output += amplitude * PureNoise(x * frequency, y * frequency, i * _fractional_increment * frequency);
				denom += amplitude;
				frequency *= _lacunarity;
				amplitude *= _persistence;
			}

			return output / denom;
		}

		inline flt Fractional(flt x, flt y, flt z) const
		{
			flt output = 0.f;
			flt denom = 0.f;
			flt frequency = _frequency;
			flt amplitude = _amplitude;

			for (ui8 i = 0; i < _octave_count; i++)
			{
				output +=
					amplitude * PureNoise(x * frequency, y * frequency, z * frequency, i * _fractional_increment * frequency);
				denom += amplitude;
				frequency *= _lacunarity;
				amplitude *= _persistence;
			}

			return output / denom;
		}

		inline flt RigidFractal(flt x) const
		{
			flt output = 0.f;
			flt denom = 0.f;
			flt frequency = _frequency;
			flt amplitude = _amplitude;

			for (ui8 i = 0; i < _octave_count; i++)
			{
				output += amplitude * ::std::abs(PureNoise(x * frequency));
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

		inline flt RigidFractal(flt x, flt y) const
		{
			flt output = 0.f;
			flt denom = 0.f;
			flt frequency = _frequency;
			flt amplitude = _amplitude;

			for (ui8 i = 0; i < _octave_count; i++)
			{
				output += amplitude * ::std::abs(PureNoise(x * frequency, y * frequency));
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

		inline flt RigidFractal(flt x, flt y, flt z) const
		{
			flt output = 0.f;
			flt denom = 0.f;
			flt frequency = _frequency;
			flt amplitude = _amplitude;

			for (ui8 i = 0; i < _octave_count; i++)
			{
				output += amplitude * ::std::abs(PureNoise(x * frequency, y * frequency, z * frequency));
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

		inline flt RigidFractal(flt x, flt y, flt z, flt w) const
		{
			flt output = 0.f;
			flt denom = 0.f;
			flt frequency = _frequency;
			flt amplitude = _amplitude;

			for (ui8 i = 0; i < _octave_count; i++)
			{
				output += amplitude * ::std::abs(PureNoise(x * frequency, y * frequency, z * frequency, w * frequency));
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

		inline flt RigidFractional(flt x) const
		{
			flt output = 0.f;
			flt denom = 0.f;
			flt frequency = _frequency;
			flt amplitude = _amplitude;

			for (ui8 i = 0; i < _octave_count; i++)
			{
				output += amplitude * ::std::abs(PureNoise(x * frequency, i * _fractional_increment * frequency));
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
					amplitude * ::std::abs(PureNoise(x * frequency, y * frequency, i * _fractional_increment * frequency));
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

		inline flt RigidFractional(flt x, flt y, flt z) const
		{
			flt output = 0.f;
			flt denom = 0.f;
			flt frequency = _frequency;
			flt amplitude = _amplitude;

			for (ui8 i = 0; i < _octave_count; i++)
			{
				output += amplitude *
					::std::abs(PureNoise(x * frequency, y * frequency, z * frequency, i * _fractional_increment * frequency));
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

		inline flt BillowFractal(flt x) const
		{
			return mat::Flip(RigidFractal(x));
		}

		inline flt BillowFractal(flt x, flt y) const
		{
			return mat::Flip(RigidFractal(x, y));
		}

		inline flt BillowFractal(flt x, flt y, flt z) const
		{
			return mat::Flip(RigidFractal(x, y, z));
		}

		inline flt BillowFractal(flt x, flt y, flt z, flt w) const
		{
			return mat::Flip(RigidFractal(x, y, z, w));
		}

		inline flt BillowFractional(flt x) const
		{
			return mat::Flip(RigidFractional(x));
		}

		inline flt BillowFractional(flt x, flt y) const
		{
			return mat::Flip(RigidFractional(x, y));
		}

		inline flt BillowFractional(flt x, flt y, flt z) const
		{
			return mat::Flip(RigidFractional(x, y, z));
		}

		inline flt WarpFractal(flt x) const
		{
			ui8 woc = _warp_octave_count;
			flt wom = _warp_octave_multiplier;
			flt woi = _warp_octave_increment;
			flt wod = _warp_octave_displacement;

			flt wx = 0; // warping x
			flt output = 0;

			// with wod = 0, ox and oy would end up being the same value, so we just use ox below
			for (ui8 i = 0; i < woc; i++)
			{
				wx = Fractal(x + wom * wx + (i + wod) * woi);
			}

			output = Fractal(x + wom * wx + woc * woi);

			return output;
		}

		inline flt WarpFractal(flt x, flt y) const
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
				{
					wx = Fractal(x + wom * wx + i * woi, y + wom * wx + i * woi);
				}

				output = Fractal(x + wom * wx + woc * woi, y + wom * wx + woc * woi);
			}
			else
			{
				for (ui8 i = 0; i < woc; i++)
				{
					wx = Fractal(x + wom * wx + i * woi, y + wom * wy + i * woi);
					wy = Fractal(x + wom * wx + (i + wod) * woi, y + wom * wy + (i + wod) * woi);
				}

				output = Fractal(x + wom * wx + woc * woi, y + wom * wy + woc * woi);
			}

			return output;
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
				{
					wx = Fractal(x + wom * wx + i * woi, y + wom * wx + i * woi, z + wom * wz + i * woi);
				}

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

		inline flt WarpFractal(flt x, flt y, flt z, flt w) const
		{
			ui8 woc = _warp_octave_count;
			flt wom = _warp_octave_multiplier;
			flt woi = _warp_octave_increment;
			flt wod = _warp_octave_displacement;

			flt wx = 0; // warping x
			flt wy = 0; // warping y
			flt wz = 0; // warping z
			flt ww = 0; // warping w
			flt output = 0;

			if (wod == 0)
			{
				// with wod = 0, ox and oy would end up being the same value, so we just use ox below
				for (ui8 i = 0; i < woc; i++)
				{
					wx =
						Fractal(x + wom * wx + i * woi, y + wom * wx + i * woi, z + wom * wz + i * woi, w + wom * ww + i * woi);
				}

				output = Fractal(x + wom * wx + woc * woi, y + wom * wx + woc * woi, z + wom * wz + woc * woi,
								 w + wom * ww + woc * woi);
			}
			else
			{
				for (ui8 i = 0; i < woc; i++)
				{
					wx =
						Fractal(x + wom * wx + i * woi, y + wom * wy + i * woi, z + wom * wz + i * woi, w + wom * ww + i * woi);
					wy = Fractal(x + wom * wx + (i + wod) * woi, y + wom * wy + (i + wod) * woi, z + wom * wz + (i + wod) * woi,
								 w + wom * ww + (i + wod) * woi);
					wz = Fractal(x + wom * wx + (i + 2 * wod) * woi, y + wom * wy + (i + 2 * wod) * woi,
								 z + wom * wz + (i + 2 * wod) * woi, w + wom * ww + (i + 2 * wod) * woi);
					ww = Fractal(x + wom * wx + (i + 4 * wod) * woi, y + wom * wy + (i + 4 * wod) * woi,
								 z + wom * wz + (i + 4 * wod) * woi, w + wom * ww + (i + 4 * wod) * woi);
				}

				output = Fractal(x + wom * wx + woc * woi, y + wom * wy + woc * woi, z + wom * wz + woc * woi,
								 w + wom * ww + woc * woi);
			}

			return output;
		}

		inline flt WarpFractional(flt x) const
		{
			ui8 woc = _warp_octave_count;
			flt wom = _warp_octave_multiplier;
			flt woi = _warp_octave_increment;
			flt wod = _warp_octave_displacement;

			flt wx = 0; // warping x
			flt output = 0;

			// with wod = 0, ox and oy would end up being the same value, so we just use ox below
			for (ui8 i = 0; i < woc; i++)
			{
				wx = Fractional(x + wom * wx + (i + wod) * woi);
			}

			output = Fractional(x + wom * wx + woc * woi);

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
				{
					wx = Fractional(x + wom * wx + i * woi, y + wom * wx + i * woi);
				}

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

		inline flt WarpFractional(flt x, flt y, flt z) const
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
				{
					wx = Fractional(x + wom * wx + i * woi, y + wom * wx + i * woi, z + wom * wz + i * woi);
				}

				output = Fractional(x + wom * wx + woc * woi, y + wom * wx + woc * woi, z + wom * wz + woc * woi);
			}
			else
			{
				for (ui8 i = 0; i < woc; i++)
				{
					wx = Fractional(x + wom * wx + i * woi, y + wom * wy + i * woi, z + wom * wz + i * woi);
					wy = Fractional(x + wom * wx + (i + wod) * woi, y + wom * wy + (i + wod) * woi,
									z + wom * wz + (i + wod) * woi);
					wz = Fractional(x + wom * wx + (i + 2 * wod) * woi, y + wom * wy + (i + 2 * wod) * woi,
									z + wom * wz + (i + 2 * wod) * woi);
				}

				output = Fractional(x + wom * wx + woc * woi, y + wom * wy + woc * woi, z + wom * wz + woc * woi);
			}

			return output;
		}
	};
} // namespace np::noiz

#endif /* NP_ENGINE_SIMPLEX_HPP */
