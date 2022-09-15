//##===----------------------------------------------------------------------===##//
//
//  Author: Nathan Phipps 9/14/22
//
//##===----------------------------------------------------------------------===##//

#ifndef NP_ENGINE_INPUT_STATE_HPP
#define NP_ENGINE_INPUT_STATE_HPP

#include "NP-Engine/Primitive/Primitive.hpp"
#include "NP-Engine/Math/Math.hpp"

namespace np::nput
{
	template <typename InputCode>
	class InputState
	{
	public:
		enum class Activity : ui32
		{
			None = 0,
			Active,
			Inactive,
			Max
		};

	private:
		InputCode _code = InputCode::Unkown;
		Activity _activity = Activity::None;
		flt _activity_level = 1.f;

	public:

		flt GetActivityLevel() const
		{
			return IsActive() ? 0.f : _activity_level;
		}

		Activity GetActivity() const
		{
			return _activity;
		}

		bl IsActive() const
		{
			return GetActivity() == Activity::Active;
		}

		bl IsInactive() const
		{
			return !IsActive();
		}

		InputCode GetCode() const
		{
			return _code;
		}

		void SetActivityLevel(flt activity_level)
		{
			_activity_level = mat::Clamp(activity_level, 0.f, 1.f);
		}

		void SetActivity(Activity activity)
		{
			_activity = activity;
		}

		void SetCode(InputCode code)
		{
			_code = code;
		}
	};
}

#endif /* NP_ENGINE_INPUT_STATE_HPP */