//##===----------------------------------------------------------------------===##//
//
//  Author: Nathan Phipps 9/14/22
//
//##===----------------------------------------------------------------------===##//

#ifndef NP_ENGINE_INPUT_QUEUE_HPP
#define NP_ENGINE_INPUT_QUEUE_HPP

#include "NP-Engine/Primitive/Primitive.hpp"
#include "NP-Engine/Container/Container.hpp"

#include "InputListener.hpp"

namespace np::nput
{
	class InputQueue : public InputListener
	{
	protected:
		using KeyCodeStateSubmissions = mutexed_wrapper<con::queue<KeyCodeState>>;
		using MouseCodeStateSubmissions = mutexed_wrapper<con::queue<MouseCodeState>>;
		using MousePositionSubmissions = mutexed_wrapper<MousePosition>;
		using ControllerCodeStateSubmissions = mutexed_wrapper<con::queue<ControllerCodeState>>;

		atm_bl _flag;

		KeyCodeStates _key_states;
		MouseCodeStates _mouse_states;
		MousePosition _mouse_position;
		ControllerCodeStates _controller_states;
		
		con::array<KeyCodeStateSubmissions, 2> _key_submissions;
		con::array<MouseCodeStateSubmissions, 2> _mouse_submissions;
		con::array<MousePositionSubmissions, 2> _mouse_position_submissions;
		con::array<ControllerCodeStateSubmissions, 2> _controller_submissions;

		KeyCodeStateSubmissions& GetKeyCodeSubmissions(bl flag)
		{
			return flag ? _key_submissions.front() : _key_submissions.back();
		}

		MouseCodeStateSubmissions& GetMouseCodeSubmissions(bl flag)
		{
			return flag ? _mouse_submissions.front() : _mouse_submissions.back();
		}

		MousePositionSubmissions& GetMousePositionSubmissions(bl flag)
		{
			return flag ? _mouse_position_submissions.front() : _mouse_position_submissions.back();
		}

		ControllerCodeStateSubmissions& GetControllerCodeSubmissions(bl flag)
		{
			return flag ? _controller_submissions.front() : _controller_submissions.back();
		}

	public:
		InputQueue(): _flag(true)
		{
			for (siz i = 0; i < (siz)KeyCode::Max; i++)
				_key_states[i].SetCode(i);

			for (siz i = 0; i < (siz)MouseCode::Max; i++)
				_mouse_states[i].SetCode(i);

			for (siz i = 0; i < (siz)ControllerCode::Max; i++)
				_controller_states[i].SetCode(i);
		}

		void ApplySubmissions()
		{
			bl flag = _flag.load(mo_acquire);
			while (!_flag.compare_exchange_weak(flag, !flag, mo_release, mo_relaxed)) {}
			
			{
				KeyCodeState state;
				auto submissions = GetKeyCodeSubmissions(flag).get_access();
				while (!submissions->empty())
				{
					state = submissions->front();
					submissions->pop();
					_key_states[state.GetCode()] = state;
				}
			}
			{
				MouseCodeState state;
				auto submissions = GetMouseCodeSubmissions(flag).get_access();
				while (!submissions->empty())
				{
					state = submissions->front();
					submissions->pop();
					_mouse_states[state.GetCode()] = state;
				}
			}
			{
				MousePosition position;
				auto submission = GetMousePositionSubmissions(flag).get_access();
				_mouse_position = *submission;
				*GetMousePositionSubmissions(!flag).get_access() = _mouse_position;
			}
			{
				ControllerCodeState state;
				auto submissions = GetControllerCodeSubmissions(flag).get_access();
				while (!submissions->empty())
				{
					state = submissions->front();
					submissions->pop();
					_controller_states[state.GetCode()] = state;
				}
			}
		}

		void Submit(const KeyCodeState& key_code_state) override
		{
			GetKeyCodeSubmissions(_flag.load(mo_acquire)).get_access()->emplace(key_code_state);
		}

		void Submit(const MouseCodeState& mouse_code_state) override
		{
			GetMouseCodeSubmissions(_flag.load(mo_acquire)).get_access()->emplace(mouse_code_state);
		}

		void Submit(const MousePosition& mouse_position) override
		{
			*GetMousePositionSubmissions(_flag.load(mo_acquire)).get_access() = mouse_position;
		}

		void Submit(const ControllerCodeState& controller_code_state) override
		{
			GetControllerCodeSubmissions(_flag.load(mo_acquire)).get_access()->emplace(controller_code_state);
		}

		const KeyCodeStates& GetKeyCodeStates() const
		{
			return _key_states;
		}

		const MousePosition& GetMousePosition() const
		{
			return _mouse_position;
		}

		const MouseCodeStates& GetMouseCodeStates() const
		{
			return _mouse_states;
		}

		const ControllerCodeStates& GetControllerCodeStates() const
		{
			return _controller_states;
		}
	};
} // namespace np::nput

#endif /* NP_ENGINE_INPUT_QUEUE_HPP */