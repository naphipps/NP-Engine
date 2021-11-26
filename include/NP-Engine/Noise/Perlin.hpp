
#ifndef NP_ENGINE_PERLIN_HPP
#define NP_ENGINE_PERLIN_HPP

#include "NP-Engine/Primitive/Primitive.hpp"
#include "NP-Engine/Random/Random.hpp"
#include "NP-Engine/Math/Math.hpp"
#include "NP-Engine/String/String.hpp"
#include "NP-Engine/Serialization/Serialization.hpp"
#include "NP-Engine/JSON/JSON.hpp"

namespace np
{
	namespace noise
	{
		/**
		 provides improved perlin noise 3D
		 provides fractal, fractional, rigid, billow, and warp methods
		 */
		class Perlin : public random::Random32Base
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
			constexpr static chr AsFilename[] = "perlin.json";

			constexpr static ui32 PERMUTATION_SIZE = 512;
			constexpr static ui32 HALF_PERMUTATION_SIZE = 256;
			ui8 _permutation[PERMUTATION_SIZE];

			ui8 _warp_octave_count; /// recommend [1, 2] - 0 gives you fbm - > 3 is not very usable due to performance but
									/// produces cool results - default to 1 for performance reasons
			flt _warp_octave_multiplier; /// similar to frequency, as this adds more and more "folds" to the warped shape -
										 /// default to 1
			flt _warp_octave_increment; /// recommend [0, 1] => [smooth, clumpy] but can be whatever - this is a very small
										/// detail - 0.19 was chosen as default
			flt _warp_octave_displacement; /// recommend [0] for performance reasons - can be any value - only change if 0 is
										   /// giving you too "regular" warping
			flt _frequency; ///< Frequency ("width") of the first octave of noise (default to 1.0)
			flt _amplitude; ///< Amplitude ("height") of the first octave of noise (default to 1.0)
			flt _lacunarity; ///< Lacunarity specifies the frequency multiplier between successive octaves (default to 2.0).
			flt _persistence; ///< Persistence is the loss of amplitude between successive octaves (usually 1/lacunarity)
			flt _fractional_increment; /// how much the fractional method increments to prevent fractal artifacts (default to
									   /// 0.19)
			ui8 _octave_count; /// recommends [3, 6] for good control of detail - inputs [6, +oo) can be way much detail and
							   /// perform slow - inputs [0, 3] may be too little detail - default to 3 for performance reasons
			ui8 _rigidity; /// higher rigidity means a more stark noise (default to 1)

			//---------------------------------------------------------------------------

			/**
			 fade method for noise method
			 */
			inline static flt Fade(flt t)
			{
				return t * t * t * (t * (t * 6 - 15) + 10);
			}

			/**
			 gradient method for 3D noise
			 */
			inline static flt Grad(i32 hash, flt x, flt y, flt z)
			{
				i32 h = hash & 15;
				// Convert lower 4 bits of hash into 12 gradient directions
				flt u = h < 8 ? x : y, v = h < 4 ? y : h == 12 || h == 14 ? x : z;
				return ((h & 1) == 0 ? u : -u) + ((h & 2) == 0 ? v : -v);
			}

			/**
			 inits the permutation with the default values
			 */
			inline void InitPermutation()
			{
				for (ui32 i = 0; i < HALF_PERMUTATION_SIZE; i++)
				{
					_permutation[i] = i;
					_permutation[HALF_PERMUTATION_SIZE + i] = i;
				}
			}

		public:
			//---------------------------------------------------------------------------

			/**
			 constructor
			 */
			Perlin(const random::Random32& engine = random::Random32()): random::Random32Base(engine)
			{
				Init();
			}

			/**
			 deconstructor
			 */
			~Perlin() {}

			//---------------------------------------------------------------------------

			/**
			 initializes the noise object -- should be done after you call SetRandomEngine
			 resets the permutation and all properties
			 frequency = 1
			 amplitude = 1
			 lacunarity = 2
			 persistence = 0.5
			 fractional increment = 0.19
			 octave count = 3
			 rigidity = 1
			 warp octave count = 1
			 warp octave multiplier = 1
			 warp octave increment = 0.19
			 warp octave displacement = 0
			 */
			inline void Init() override
			{
				random::Random32Base::Init();

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

			/**
			 shuffles the internal permutation
			 */
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

			/**
			 serialization method for us to write
			 we require our objects to know which filepath they are a part of
			 */
			virtual ostrm& Insertion(ostrm& os, str filepath) const override
			{
				nlohmann::json json;

				str random32base_path = fs::append(fs::get_parent_path(filepath), "random32base_path");
				json["random32base_path"] = random32base_path;

				json["frequency"] = _frequency;
				json["amplitude"] = _amplitude;
				json["lacunarity"] = _lacunarity;
				json["persistence"] = _persistence;
				json["fractional_increment"] = _fractional_increment;
				json["octave_count"] = _octave_count;
				json["rigidity"] = _rigidity;
				json["warp_octave_count"] = _warp_octave_count;
				json["warp_octace_multiplier"] = _warp_octave_multiplier;
				json["warp_octave_increment"] = _warp_octave_increment;
				json["warp_octave_displacement"] = _warp_octave_displacement;

				json["permutation"] = nlohmann::json::array();
				for (i32 i = 0; i < HALF_PERMUTATION_SIZE; i++)
				{
					json["permutation"].push_back(_permutation[i]);
				}

				os << json.dump(NP_JSON_SPACING);
				random::Random32Base::SaveTo(random32base_path);

				return os;
			}

			/**
			 deserialization method for us to read
			 we require our objects to know which filepath they are a part of
			 */
			virtual istrm& Extraction(istrm& is, str filepath) override
			{
				nlohmann::json json;
				is >> json;

				_frequency = json["frequency"];
				_amplitude = json["amplitude"];
				_lacunarity = json["lacunarity"];
				_persistence = json["persistence"];
				_fractional_increment = json["fractional_increment"];
				_octave_count = json["octave_count"];
				_rigidity = json["rigidity"];
				_warp_octave_count = json["warp_octave_count"];
				_warp_octave_multiplier = json["warp_octace_multiplier"];
				_warp_octave_increment = json["warp_octave_increment"];
				_warp_octave_displacement = json["warp_octave_displacement"];

				for (i32 i = 0; i < HALF_PERMUTATION_SIZE; i++)
				{
					_permutation[i] = json["permutation"][i];
					_permutation[HALF_PERMUTATION_SIZE + i] = _permutation[i];
				}

				str random32base_path = json["random32base_path"];
				random::Random32Base::LoadFrom(random32base_path);

				return is;
			}

			/**
			 save oursellves inside the given dirpath
			 return if the save was successful or not
			 */
			virtual bl SaveTo(str dirpath) const override
			{
				return Perlin::template SaveAs<Perlin>(fs::append(dirpath, AsFilename), this);
			}

			/**
			 load outselves from the given dirpath
			 return if the load was successful or not
			 */
			virtual bl LoadFrom(str dirpath) override
			{
				return Perlin::template LoadAs<Perlin>(fs::append(dirpath, AsFilename), this);
			}

			//---------------------------------------------------------------------------

			/**
			 sets frequency
			 */
			inline void SetFrequency(flt f)
			{
				_frequency = f;
			}

			/**
			 gets frequency
			 */
			inline flt GetFrequency() const
			{
				return _frequency;
			}

			/**
			 sets amplitude
			 */
			inline void SetAmplitude(flt a)
			{
				_amplitude = a;
			}

			/**
			 gets amplitude
			 */
			inline flt GetAmplitude() const
			{
				return _amplitude;
			}

			/**
			 sets lacunarity
			 */
			inline void SetLacunarity(flt l)
			{
				_lacunarity = l;
			}

			/**
			 gets lacunarity
			 */
			inline flt GetLacunarity() const
			{
				return _lacunarity;
			}

			/**
			 sets persistence
			 */
			inline void SetPersistence(flt p)
			{
				_persistence = p;
			}

			/**
			 gets persistence
			 */
			inline flt GetPersistence() const
			{
				return _persistence;
			}

			/**
			 sets fractional increment
			 */
			inline void SetFractionalIncrement(flt f)
			{
				_fractional_increment = f;
			}

			/**
			 gets fractional increment
			 */
			inline flt GetFractionalIncrement() const
			{
				return _fractional_increment;
			}

			/**
			 sets octave count
			 */
			inline void SetOctaveCount(ui8 o)
			{
				_octave_count = o;
			}

			/**
			 gets octave count
			 */
			inline ui8 GetOctaveCount() const
			{
				return _octave_count;
			}

			/**
			 sets rigidity
			 inputs [0, 8] will use high performance pow, [9, 255] will use std::pow
			 */
			inline void SetRigidity(ui8 r)
			{
				_rigidity = r;
			}

			/**
			 gets rigidity
			 */
			inline ui8 GetRigidity() const
			{
				return _rigidity;
			}

			/**
			 sets the warp octave count
			 recommend inputs [1, 2] - input  0 gives you fbm - inputs [3, +oo) is not very usable due to performance but
			 produces cool results
			 */
			inline void SetWarpOctaveCount(ui8 count)
			{
				_warp_octave_count = count;
			}

			/**
			 gets warp octave count
			 */
			inline ui8 GetWarpOctaveCount() const
			{
				return _warp_octave_count;
			}

			/**
			 sets warp octave multiplier
			 similar to frequency, as this adds more and more "folds" to the warped shape
			 inputs recommend staying around 1 as in recommend inputs [0.5, 1.5] - can be anything
			 */
			inline void SetWarpOctaveMultiplier(flt multiplier)
			{
				_warp_octave_multiplier = multiplier;
			}

			/**
			 gets warp octave multiplier
			 */
			inline flt GetWarpOctaveMultiplier() const
			{
				return _warp_octave_multiplier;
			}

			/**
			 sets warp octave increment
			 recommend inputs [0, 1] => [smooth, clumpy] but can be whatever - this is a very small detail - 0.19 was chosen as
			 default
			 */
			inline void SetWarpOctaveIncrement(flt increment)
			{
				_warp_octave_increment = increment;
			}

			/**
			 gest warp octave increment
			 */
			inline flt GetWarpOctaveIncrement() const
			{
				return _warp_octave_increment;
			}

			/**
			 sets warp octave displacement
			 recommend [0] for performance reasons - can be any value - recommend to change iff 0 is giving you too "regular" or
			 "boring" warping and if so then recommend arbitrary input 0.19
			 */
			inline void SetWarpOctaveDisplacement(flt displacement)
			{
				_warp_octave_displacement = displacement;
			}

			/**
			 gest warp octave displacement
			 */
			inline flt GetWarpOctaveDisplacement() const
			{
				return _warp_octave_displacement;
			}

			//---------------------------------------------------------------------------

			/**
			 noise 3D
			 output range [-amplitude, amplitude]
			 */
			inline flt Noise(flt x, flt y, flt z) const
			{
				return GetAmplitude() * CalculateNoise(GetFrequency() * x, GetFrequency() * y, GetFrequency() * z);
			}

			/**
			 noise 3D
			 output range [-1, 1]
			 */
			inline flt CalculateNoise(flt x, flt y, flt z) const
			{
				// Find the unit cube that contains the point
				i32 X = math::fastfloor(x);
				i32 Y = math::fastfloor(y);
				i32 Z = math::fastfloor(z);

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
				flt res = math::lerp(
					math::lerp(math::lerp(Grad(_permutation[AA], x, y, z), Grad(_permutation[BA], x - 1, y, z), u),
							   math::lerp(Grad(_permutation[AB], x, y - 1, z), Grad(_permutation[BB], x - 1, y - 1, z), u), v),
					math::lerp(
						math::lerp(Grad(_permutation[AA + 1], x, y, z - 1), Grad(_permutation[BA + 1], x - 1, y, z - 1), u),
						math::lerp(Grad(_permutation[AB + 1], x, y - 1, z - 1), Grad(_permutation[BB + 1], x - 1, y - 1, z - 1),
								   u),
						v),
					w);

				return res;
			}

			/**
			 fractal noise 3D - the same layer of noise will be added on top of itself an octave count number of times with
			 detail respect to lacunarity Note: fractal noise may contain artifacts due to it layers the same layer of noise on
			 top of itself output range [-1, 1]
			 */
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

			/**
			 fractional noise 2D - adds an octave count number of layers of noise together with detail respect to lacunarity -
			 the added layers are fractional incremenet's value away from each other in 3D Note: if fractional increment is
			 zerp, then fractional noise may contain artifacts due to it layers the same layer of noise on top of itself output
			 range [-1, 1]
			 */
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

			/**
			 rigid fractal noise 3D - provides the same noise as fractal 3D but adds the abs value of each layer then subtracts
			 it from 1 then raises it to the rigidity value's power output range [0, 1]
			 */
			inline flt RigidFractal(flt x, flt y, flt z) const
			{
				flt output = 0.f;
				flt denom = 0.f;
				flt frequency = _frequency;
				flt amplitude = _amplitude;

				for (ui8 i = 0; i < _octave_count; i++)
				{
					output += amplitude * math::abs(CalculateNoise(x * frequency, y * frequency, z * frequency));
					denom += amplitude;
					frequency *= _lacunarity;
					amplitude *= _persistence;
				}

				output /= denom;
				output = math::flip(output);

				switch (_rigidity)
				{
				case 0:
					output = math::pow0(output);
					break;
				case 1:
					output = math::pow1(output);
					break;
				case 2:
					output = math::pow2(output);
					break;
				case 3:
					output = math::pow3(output);
					break;
				case 4:
					output = math::pow4(output);
					break;
				case 5:
					output = math::pow5(output);
					break;
				case 6:
					output = math::pow6(output);
					break;
				case 7:
					output = math::pow7(output);
					break;
				case 8:
					output = math::pow8(output);
					break;
				default:
					output = math::pow(output, _rigidity);
					break;
				}

				return output;
			}

			/**
			 rigid fractional noise 2D - provides the same noise as fractional 2D but adds the abs value of each layer then
			 subtracts it from 1 then raises it to the rigidity value's power output range [0, 1]
			 */
			inline flt RigidFractional(flt x, flt y) const
			{
				flt output = 0.f;
				flt denom = 0.f;
				flt frequency = _frequency;
				flt amplitude = _amplitude;

				for (ui8 i = 0; i < _octave_count; i++)
				{
					output += amplitude *
						math::abs(CalculateNoise(x * frequency, y * frequency, i * _fractional_increment * frequency));
					denom += amplitude;
					frequency *= _lacunarity;
					amplitude *= _persistence;
				}

				output /= denom;
				output = math::flip(output);

				switch (_rigidity)
				{
				case 0:
					output = math::pow0(output);
					break;
				case 1:
					output = math::pow1(output);
					break;
				case 2:
					output = math::pow2(output);
					break;
				case 3:
					output = math::pow3(output);
					break;
				case 4:
					output = math::pow4(output);
					break;
				case 5:
					output = math::pow5(output);
					break;
				case 6:
					output = math::pow6(output);
					break;
				case 7:
					output = math::pow7(output);
					break;
				case 8:
					output = math::pow8(output);
					break;
				default:
					output = math::pow(output, _rigidity);
					break;
				}

				return output;
			}

			/**
			 billow fractal noise 3D - provides the same noise as rigid fractal 3D but subtracts it from 1
			 */
			inline flt BillowFractal(flt x, flt y, flt z) const
			{
				return math::flip(RigidFractal(x, y, z));
			}

			/**
			 billow fractional 2D - provides the same noise as rigid fractional 2D but subtracts it from 1
			 */
			inline flt BillowFractional(flt x, flt y) const
			{
				return math::flip(RigidFractional(x, y));
			}

			/**
			 warpped fractal 3D - provides the same noise as fractal 3D but warps it through itself based on warp related fields
			 */
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
						wy = Fractal(x + wom * wx + (i + wod) * woi, y + wom * wy + (i + wod) * woi,
									 z + wom * wz + (i + wod) * woi);
						wz = Fractal(x + wom * wx + (i + 2 * wod) * woi, y + wom * wy + (i + 2 * wod) * woi,
									 z + wom * wz + (i + 2 * wod) * woi);
					}

					output = Fractal(x + wom * wx + woc * woi, y + wom * wy + woc * woi, z + wom * wz + woc * woi);
				}

				return output;
			}

			/**
			 warpped fractional 2D - provides the same noise as fractional 2D but warps it through itself based on warp related
			 fields
			 */
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
		};
	} // namespace noise
} // namespace np

#endif /* NP_ENGINE_PERLIN_HPP */
