//##===----------------------------------------------------------------------===##//
//
//  Author: Nathan Phipps 9/14/22
//
//##===----------------------------------------------------------------------===##//

#ifndef NP_ENGINE_INPUT_STATE_HPP
#define NP_ENGINE_INPUT_STATE_HPP

#include "NP-Engine/Primitive/Primitive.hpp"
#include "NP-Engine/Container/Container.hpp"
#include "NP-Engine/Math/Math.hpp"

namespace np::nput
{
	template <typename InputCode>
	class InputState
	{
	private:
		InputCode _code = InputCode::Unkown;
		bl _is_active = false;
		flt _activity_level = 1.f;

	public:
		flt GetActivityLevel() const
		{
			return _activity_level;
		}

		bl IsActive() const
		{
			return _is_active;
		}

		InputCode GetCode() const
		{
			return _code;
		}

		void SetActivityLevel(flt activity_level)
		{
			_activity_level = ::std::clamp(activity_level, 0.f, 1.f);
			_is_active = _activity_level != 0.f;
		}

		void SetIsActive(bl is_active)
		{
			_is_active = is_active;
			if (_is_active)
			{
				if (_activity_level == 0.f)
					_activity_level = 1.f;
			}
			else
			{
				_activity_level = 0.f;
			}
		}

		void SetCode(InputCode code)
		{
			_code = code;
		}

		void SetCode(siz code)
		{
			_code = (InputCode)code;
		}
	};

	template <typename InputCode, siz SIZE>
	class InputStates : public con::array<InputState<InputCode>, SIZE>
	{
	public:
		InputState<InputCode>& operator[](siz index)
		{
			return con::array<InputState<InputCode>, SIZE>::operator[](index);
		}

		const InputState<InputCode>& operator[](siz index) const
		{
			return con::array<InputState<InputCode>, SIZE>::operator[](index);
		}

		InputState<InputCode>& operator[](InputCode code)
		{
			return operator[]((siz)code);
		}

		const InputState<InputCode>& operator[](InputCode code) const
		{
			return operator[]((siz)code);
		}
	};
} // namespace np::nput

#endif /* NP_ENGINE_INPUT_STATE_HPP */