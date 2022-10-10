//##===----------------------------------------------------------------------===##//
//
//  Author: Nathan Phipps 9/14/22
//
//##===----------------------------------------------------------------------===##//

#ifndef NP_ENGINE_INPUT_QUEUE_HPP
#define NP_ENGINE_INPUT_QUEUE_HPP

#include "NP-Engine/Primitive/Primitive.hpp"
#include "NP-Engine/Container/Container.hpp"

#include "NP-Engine/Insight/Insight.hpp" //TODO: remove
#include "NP-Engine/String/String.hpp" //TODO: remove

#include "NP-Engine/Vendor/GlmInclude.hpp"

#include "KeyCode.hpp"
#include "MouseCode.hpp"
#include "ControllerCode.hpp"
#include "MousePosition.hpp"

namespace np::nput
{
	class InputQueue
	{
	public:
		static void SubmitKeyCallback(void* caller, const KeyCodeState& state)
		{
			((InputQueue*)caller)->Submit(state);
		}

		static void SubmitMouseCallback(void* caller, const MouseCodeState& state)
		{
			((InputQueue*)caller)->Submit(state);
		}

		static void SubmitMousePositionCallback(void* caller, const MousePosition& position)
		{
			((InputQueue*)caller)->Submit(position);
		}

		static void SubmitControllerCallback(void* caller, const ControllerCodeState& state)
		{
			((InputQueue*)caller)->Submit(state);
		}

	protected:
		using KeyCodeStateBuffer = con::mpmc_queue<KeyCodeState>;
		using MouseCodeStateBuffer = con::mpmc_queue<MouseCodeState>;
		using ControllerCodeStateBuffer = con::mpmc_queue<ControllerCodeState>;

		atm_bl _flag;

		KeyCodeStates _key_states;
		MouseCodeStates _mouse_states;
		ControllerCodeStates _controller_states;
		MousePosition _mouse_position;

		con::array<KeyCodeStateBuffer, 2> _key_buffers;
		con::array<MouseCodeStateBuffer, 2> _mouse_buffers;
		con::array<ControllerCodeStateBuffer, 2> _controller_buffers;
		con::array<MousePosition, 2> _mouse_position_buffer;

		KeyCodeStateBuffer& GetKeyCodeBuffer(bl flag)
		{
			return flag ? _key_buffers.front() : _key_buffers.back();
		}

		MouseCodeStateBuffer& GetMouseCodeBuffer(bl flag)
		{
			return flag ? _mouse_buffers.front() : _mouse_buffers.back();
		}

		ControllerCodeStateBuffer& GetControllerCodeBuffer(bl flag)
		{
			return flag ? _controller_buffers.front() : _controller_buffers.back();
		}

		MousePosition& GetMousePositionBuffer(bl flag)
		{
			return flag ? _mouse_position_buffer.front() : _mouse_position_buffer.back();
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

			KeyCodeStateBuffer& key_other_buffer = GetKeyCodeBuffer(flag);
			MouseCodeStateBuffer& mouse_other_buffer = GetMouseCodeBuffer(flag);
			ControllerCodeStateBuffer& controller_other_buffer = GetControllerCodeBuffer(flag);

			KeyCodeState key_state;
			while (key_other_buffer.try_dequeue(key_state))
				_key_states[key_state.GetCode()] = key_state;

			MouseCodeState mouse_state;
			while (mouse_other_buffer.try_dequeue(mouse_state))
				_mouse_states[mouse_state.GetCode()] = mouse_state;

			ControllerCodeState controller_state;
			while (controller_other_buffer.try_dequeue(controller_state))
				_controller_states[controller_state.GetCode()] = controller_state;

			_mouse_position = GetMousePositionBuffer(flag);
			GetMousePositionBuffer(!flag) = _mouse_position;

			// TODO: remove log below
			/*

			str mouse_msg = "Mouse Position: ";
			str mouse_position_msg = "(" + to_str(_mouse_position.GetPosition().x) +
				", " + to_str(_mouse_position.GetPosition().y) + ")";
			mouse_msg += _mouse_position.IsOverSurface() ? mouse_position_msg : "(outside surface)";
			mouse_msg += "\n";

			mouse_msg += "Mouse Buttons: ";
			for (auto it = _mouse_states.begin(); it != _mouse_states.end(); it++)
				mouse_msg += it->GetActivity() == MouseCodeState::Activity::Active ? to_str(it->GetCode()) + ", " : "";
			mouse_msg += "\n";

			str keys_msg = "Active keys: ";
			for (auto it = _key_states.begin(); it != _key_states.end(); it++)
				keys_msg += it->GetActivity() == KeyCodeState::Activity::Active ? to_str(it->GetCode()) + ", " : "";
			keys_msg += "\n";

			NP_ENGINE_LOG_INFO(mouse_msg + keys_msg);

			//*/
		}

		void Submit(const KeyCodeState& key_code_state)
		{
			GetKeyCodeBuffer(_flag.load(mo_acquire)).enqueue(key_code_state);
		}

		void Submit(const MouseCodeState& mouse_code_state)
		{
			GetMouseCodeBuffer(_flag.load(mo_acquire)).enqueue(mouse_code_state);
		}

		void Submit(const ControllerCodeState& controller_code_state)
		{
			GetControllerCodeBuffer(_flag.load(mo_acquire)).enqueue(controller_code_state);
		}

		void Submit(const MousePosition& mouse_position)
		{
			GetMousePositionBuffer(_flag.load(mo_acquire)) = mouse_position;
		}

		const KeyCodeStates& GetKeyCodeStates() const
		{
			return _key_states;
		}

		const MouseCodeStates& GetMouseCodeStates() const
		{
			return _mouse_states;
		}

		const ControllerCodeStates& GetControllerCodeStates() const
		{
			return _controller_states;
		}

		const MousePosition& GetMousePosition() const
		{
			return _mouse_position;
		}
	};
} // namespace np::nput

#endif /* NP_ENGINE_INPUT_QUEUE_HPP */