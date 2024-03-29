//##===----------------------------------------------------------------------===##//
//
//  Author: Nathan Phipps 3/7/20
//
//##===----------------------------------------------------------------------===##//

#ifndef NP_ENGINE_CLIP_LINE_HPP
#define NP_ENGINE_CLIP_LINE_HPP

#include "NP-Engine/Foundation/Foundation.hpp"
#include "NP-Engine/Primitive/Primitive.hpp"
#include "NP-Engine/String/String.hpp"
#include "NP-Engine/Math/Math.hpp"

namespace np::alg
{
	/*
		Implementation of the Sobkow-Pospisil-Yang clipping algorithm, with fixed clipping functions,
		and a return struct providing more insight.

		Citation:
		Sobkow, M., Pospisila, P., & Yang, Y. (1987). A fast two-dimesional line clipping algorithm via line encoding. Compuing
		and Graphics, 11(4), 459-467
	*/

	namespace __detail
	{
		template <typename T>
		static inline bl ClipPTop(::glm::vec<2, T>& p, const ::glm::vec<2, T>& q, const T top)
		{
			::glm::vec<2, T> temp = p;
			p.x += (q.x - p.x) * (top - p.y) / (q.y - p.y);
			p.y = top;
			return temp != p;
		}

		template <typename T>
		static inline bl ClipPBottom(::glm::vec<2, T>& p, const ::glm::vec<2, T>& q, const T bottom)
		{
			::glm::vec<2, T> temp = p;
			p.x += (q.x - p.x) * (bottom - p.y) / (q.y - p.y);
			p.y = bottom;
			return temp != p;
		}

		template <typename T>
		static inline bl ClipPRight(::glm::vec<2, T>& p, const ::glm::vec<2, T>& q, const T right)
		{
			::glm::vec<2, T> temp = p;
			p.y += (q.y - p.y) * (right - p.x) / (q.x - p.x);
			p.x = right;
			return temp != p;
		}

		template <typename T>
		static inline bl ClipPLeft(::glm::vec<2, T>& p, const ::glm::vec<2, T>& q, const T left)
		{
			::glm::vec<2, T> temp = p;
			p.y += (q.y - p.y) * (left - p.x) / (q.x - p.x);
			p.x = left;
			return temp != p;
		}

		template <typename T>
		static inline bl ClipQTop(const ::glm::vec<2, T>& p, ::glm::vec<2, T>& q, const T top)
		{
			::glm::vec<2, T> temp = q;
			q.x += (p.x - q.x) * (top - q.y) / (p.y - q.y);
			q.y = top;
			return temp != q;
		}

		template <typename T>
		static inline bl ClipQBottom(const ::glm::vec<2, T>& p, ::glm::vec<2, T>& q, const T bottom)
		{
			::glm::vec<2, T> temp = q;
			q.x += (p.x - q.x) * (bottom - q.y) / (p.y - q.y);
			q.y = bottom;
			return temp != q;
		}

		template <typename T>
		static inline bl ClipQRight(const ::glm::vec<2, T>& p, ::glm::vec<2, T>& q, const T right)
		{
			::glm::vec<2, T> temp = q;
			q.y += (p.y - q.y) * (right - q.x) / (p.x - q.x);
			q.x = right;
			return temp != q;
		}

		template <typename T>
		static inline bl ClipQLeft(const ::glm::vec<2, T>& p, ::glm::vec<2, T>& q, const T left)
		{
			::glm::vec<2, T> temp = q;
			q.y += (p.y - q.y) * (left - q.x) / (p.x - q.x);
			q.x = left;
			return temp != q;
		}
	} // namespace __detail

	/*
		return struct for method ClipLine. Indicates if the line was clipped and is line is visible within given bounds
	*/
	struct ClipLineReturn
	{
		bl visible = false;
		bl clipped = false;
	};

	/*
		clips given line pq given top, right, bottom, and left value
	*/
	template <typename T>
	static inline ClipLineReturn ClipLine(::glm::vec<2, T>& p, ::glm::vec<2, T>& q, T top, T right, T bottom, T left)
	{
		ClipLineReturn retval;

		/*
			line_code key
			--------------------------
			|   $9   |   $8   |   $A   |
			|  1001  |  1000  |  1010  |
			|--------+--------+--------|
			|   $1   |   $0   |   $2   |
			|  0001  |  0000  |  0010  |
			|--------+--------+--------|
			|   $5   |   $4   |   $6   |
			|  0101  |  0100  |  0110  |
			--------------------------
			*/

		ui8 line_code = 0;
		line_code += q.y > top ? 8 : 0;
		line_code += q.y < bottom ? 4 : 0;
		line_code += q.x > right ? 2 : 0;
		line_code += q.x < left ? 1 : 0;
		line_code += p.y > top ? 128 : 0;
		line_code += p.y < bottom ? 64 : 0;
		line_code += p.x > right ? 32 : 0;
		line_code += p.x < left ? 16 : 0;

		switch (line_code)
		{
		case 0x00:
			retval.visible = true;
			break;

		case 0x01:
			retval.clipped = __detail::ClipQLeft(p, left);
			retval.visible = true;
			break;

		case 0x02:
			retval.clipped = __detail::ClipQRight(p, right);
			retval.visible = true;
			break;

		case 0x04:
			retval.clipped = __detail::ClipQBottom(p, bottom);
			retval.visible = true;
			break;

		case 0x05:
			retval.clipped = __detail::ClipQLeft(p, left);
			if (q.y < bottom)
				retval.clipped |= __detail::ClipQBottom(p, bottom);
			retval.visible = true;
			break;

		case 0x06:
			retval.clipped = __detail::ClipQRight(p, right);
			if (q.y < bottom)
				retval.clipped |= __detail::ClipQBottom(p, bottom);
			retval.visible = true;
			break;

		case 0x08:
			retval.clipped = __detail::ClipQTop(p, top);
			retval.visible = true;
			break;

		case 0x09:
			retval.clipped = __detail::ClipQLeft(p, left);
			if (q.y > top)
				retval.clipped |= __detail::ClipQTop(p, top);
			retval.visible = true;
			break;

		case 0x0A:
			retval.clipped = __detail::ClipQRight(p, right);
			if (q.y > top)
				retval.clipped |= __detail::ClipQTop(p, top);
			retval.visible = true;
			break;

			//---------------------------------------------

		case 0x10:
			retval.clipped = __detail::ClipPLeft(p, left);
			retval.visible = true;
			break;

		case 0x11:
			break;

		case 0x12:
			retval.clipped = __detail::ClipPLeft(p, left);
			retval.clipped |= __detail::ClipQRight(p, right);
			retval.visible = true;
			break;

		case 0x14:
			retval.clipped = __detail::ClipPLeft(p, left);
			if (p.y >= bottom)
			{
				retval.clipped |= __detail::ClipQBottom(p, bottom);
				retval.visible = true;
			}
			break;

		case 0x15:
			break;

		case 0x16:
			retval.clipped = __detail::ClipPLeft(p, left);
			if (p.y >= bottom)
			{
				retval.clipped |= __detail::ClipQBottom(p, bottom);
				if (q.x > right)
					retval.clipped |= __detail::ClipQRight(p, right);
				retval.visible = true;
			}
			break;

		case 0x18:
			retval.clipped = __detail::ClipPLeft(p, left);
			if (p.y <= top)
			{
				retval.clipped |= __detail::ClipQTop(p, top);
				retval.visible = true;
			}
			break;

		case 0x19:
			break;

		case 0x1A:
			retval.clipped = __detail::ClipPLeft(p, left);
			if (p.y <= top)
			{
				retval.clipped |= __detail::ClipQTop(p, top);
				if (q.x > right)
					retval.clipped |= __detail::ClipQRight(p, right);
				retval.visible = true;
			}
			break;

			//---------------------------------------------

		case 0x20:
			retval.clipped = __detail::ClipPRight(p, right);
			retval.visible = true;
			break;

		case 0x21:
			retval.clipped = __detail::ClipPRight(p, right);
			retval.clipped |= __detail::ClipQLeft(p, left);
			retval.visible = true;
			break;

		case 0x22:
			break;

		case 0x24:
			retval.clipped = __detail::ClipPRight(p, right);
			if (p.y >= bottom)
			{
				retval.clipped |= __detail::ClipQBottom(p, bottom);
				retval.visible = true;
			}
			break;

		case 0x25:
			retval.clipped = __detail::ClipPRight(p, right);
			if (p.y >= bottom)
			{
				retval.clipped |= __detail::ClipQBottom(p, bottom);
				if (q.x < left)
					retval.clipped |= __detail::ClipQLeft(p, left);
				retval.visible = true;
			}
			break;

		case 0x26:
			break;

		case 0x28:
			retval.clipped = __detail::ClipPRight(p, right);
			if (p.y <= top)
			{
				retval.clipped |= __detail::ClipQTop(p, top);
				retval.visible = true;
			}
			break;

		case 0x29:
			retval.clipped = __detail::ClipPRight(p, right);
			if (p.y <= top)
			{
				retval.clipped |= __detail::ClipQTop(p, top);
				if (q.x < left)
					retval.clipped |= __detail::ClipQLeft(p, left);
				retval.visible = true;
			}
			break;

		case 0x2A:
			break;

			//---------------------------------------------

		case 0x40:
			retval.clipped = __detail::ClipPBottom(p, bottom);
			retval.visible = true;
			break;

		case 0x41:
			retval.clipped = __detail::ClipPBottom(p, bottom);
			if (p.x >= left)
			{
				retval.clipped |= __detail::ClipQLeft(p, left);
				if (q.y < bottom)
					retval.clipped |= __detail::ClipQBottom(p, bottom);
				retval.visible = true;
			}
			break;

		case 0x42:
			retval.clipped = __detail::ClipPBottom(p, bottom);
			if (p.x <= right)
			{
				retval.clipped |= __detail::ClipQRight(p, right);
				retval.visible = true;
			}
			break;

		case 0x44:
			break;

		case 0x45:
			break;

		case 0x46:
			break;

		case 0x48:
			retval.clipped = __detail::ClipPBottom(p, bottom);
			retval.clipped |= __detail::ClipQTop(p, top);
			retval.visible = true;
			break;

		case 0x49:
			retval.clipped = __detail::ClipPBottom(p, bottom);
			if (p.x >= left)
			{
				retval.clipped |= __detail::ClipQLeft(p, left);
				if (q.y > top)
					retval.clipped |= __detail::ClipQTop(p, top);
				retval.visible = true;
			}
			break;

		case 0x4A:
			retval.clipped = __detail::ClipPBottom(p, bottom);
			if (p.x <= right)
			{
				retval.clipped |= __detail::ClipQRight(p, right);
				if (q.y > top)
					retval.clipped |= __detail::ClipQTop(p, top);
				retval.visible = true;
			}
			break;

			//---------------------------------------------

		case 0x50:
			retval.clipped = __detail::ClipPLeft(p, left);
			if (p.y < bottom)
				retval.clipped |= __detail::ClipPBottom(p, bottom);
			retval.visible = true;
			break;

		case 0x51:
			break;

		case 0x52:
			retval.clipped = __detail::ClipQRight(p, right);
			if (q.y >= bottom)
			{
				retval.clipped |= __detail::ClipPBottom(p, bottom);
				if (p.x < left)
					retval.clipped |= __detail::ClipPLeft(p, left);
				retval.visible = true;
			}
			break;

		case 0x54:
			break;

		case 0x55:
			break;

		case 0x56:
			break;

		case 0x58:
			retval.clipped = __detail::ClipQTop(p, top);
			if (q.x >= left)
			{
				retval.clipped |= __detail::ClipPBottom(p, bottom);
				if (p.x < left)
					retval.clipped |= __detail::ClipPLeft(p, left);
				retval.visible = true;
			}

		case 0x59:
			break;

		case 0x5A:
			retval.clipped = __detail::ClipPLeft(p, left);
			if (p.y >= left)
			{
				retval.clipped |= __detail::ClipQRight(p, right);
				if (q.y >= bottom)
				{
					if (p.y < bottom)
						retval.clipped |= __detail::ClipPBottom(p, bottom);
					if (q.y > top)
						retval.clipped |= __detail::ClipQTop(p, top);
					retval.visible = true;
				}
			}
			break;

			//---------------------------------------------

		case 0x60:
			retval.clipped = __detail::ClipPRight(p, right);
			if (p.y < bottom)
				retval.clipped |= __detail::ClipPBottom(p, bottom);
			retval.visible = true;
			break;

		case 0x61:
			retval.clipped = __detail::ClipQLeft(p, left);
			if (q.y >= bottom)
			{
				retval.clipped |= __detail::ClipPBottom(p, bottom);
				if (p.x > right)
					retval.clipped |= __detail::ClipPRight(p, right);
				retval.visible = true;
			}
			break;

		case 0x62:
			break;

		case 0x64:
			break;

		case 0x65:
			break;

		case 0x66:
			break;

		case 0x68:
			retval.clipped = __detail::ClipQTop(p, top);
			if (q.x <= right)
			{
				retval.clipped |= __detail::ClipPRight(p, right);
				if (p.y < bottom)
					retval.clipped |= __detail::ClipPBottom(p, bottom);
				retval.visible = true;
			}
			break;

		case 0x69:
			retval.clipped = __detail::ClipQLeft(p, left);
			if (q.y >= bottom)
			{
				retval.clipped |= __detail::ClipPRight(p, right);
				if (p.y <= top)
				{
					if (q.y > top)
						retval.clipped |= __detail::ClipQTop(p, top);
					if (p.y < bottom)
						retval.clipped |= __detail::ClipPBottom(p, bottom);
					retval.visible = true;
				}
			}
			break;

		case 0x6A:
			break;

			//---------------------------------------------

		case 0x80:
			retval.clipped = __detail::ClipPTop(p, top);
			retval.visible = true;
			break;

		case 0x81:
			retval.clipped = __detail::ClipPTop(p, top);
			if (p.x >= left)
			{
				retval.clipped |= __detail::ClipQLeft(p, left);
				retval.visible = true;
			}
			break;

		case 0x82:
			retval.clipped = __detail::ClipPTop(p, top);
			if (p.x <= right)
			{
				retval.clipped |= __detail::ClipQRight(p, right);
				retval.visible = true;
			}
			break;

		case 0x84:
			retval.clipped = __detail::ClipPTop(p, top);
			retval.clipped |= __detail::ClipQBottom(p, bottom);
			retval.visible = true;
			break;

		case 0x85:
			retval.clipped = __detail::ClipPTop(p, top);
			if (p.x >= left)
			{
				retval.clipped |= __detail::ClipQLeft(p, left);
				if (q.y < bottom)
					retval.clipped |= __detail::ClipQBottom(p, bottom);
				retval.visible = true;
			}
			break;

		case 0x86:
			retval.clipped = __detail::ClipPTop(p, top);
			if (p.x <= right)
			{
				retval.clipped |= __detail::ClipQRight(p, right);
				if (q.y < bottom)
					retval.clipped |= __detail::ClipQBottom(p, bottom);
				retval.visible = true;
			}
			break;

		case 0x88:
			break;

		case 0x89:
			break;

		case 0x8A:
			break;

			//---------------------------------------------

		case 0x90:
			retval.clipped = __detail::ClipPLeft(p, left);
			if (p.y > top)
				retval.clipped |= __detail::ClipPTop(p, top);
			retval.visible = true;
			break;

		case 0x91:
			break;

		case 0x92:
			retval.clipped = __detail::ClipQRight(p, right);
			if (q.y <= top)
			{
				retval.clipped |= __detail::ClipPTop(p, top);
				if (p.x < left)
					retval.clipped |= __detail::ClipPLeft(p, left);
				retval.visible = true;
			}
			break;

		case 0x94:
			retval.clipped = __detail::ClipQBottom(p, bottom);
			if (q.x >= left)
			{
				retval.clipped |= __detail::ClipPLeft(p, left);
				if (p.y > top)
					retval.clipped |= __detail::ClipPTop(p, top);
				retval.visible = true;
			}
			break;

		case 0x95:
			break;

		case 0x96:
			retval.clipped = __detail::ClipPLeft(p, left);
			if (p.y >= bottom)
			{
				retval.clipped |= __detail::ClipQRight(p, right);
				if (q.y <= top)
				{
					if (p.y > top)
						retval.clipped |= __detail::ClipPTop(p, top);
					if (q.y < bottom)
						retval.clipped |= __detail::ClipQBottom(p, bottom);
					retval.visible = true;
				}
			}
			break;

		case 0x98:
			break;

		case 0x99:
			break;

		case 0x9A:
			break;

			//---------------------------------------------

		case 0xA0:
			retval.clipped = __detail::ClipPRight(p, right);
			if (p.y > top)
				retval.clipped |= __detail::ClipPTop(p, top);
			retval.visible = true;
			break;

		case 0xA1:
			retval.clipped = __detail::ClipQLeft(p, left);
			if (q.y <= top)
			{
				retval.clipped |= __detail::ClipPTop(p, top);
				if (p.x > right)
					retval.clipped |= __detail::ClipPRight(p, right);
				retval.visible = true;
			}
			break;

		case 0xA2:
			break;

		case 0xA4:
			retval.clipped = __detail::ClipQBottom(p, bottom);
			if (q.x <= right)
			{
				retval.clipped |= __detail::ClipPRight(p, right);
				if (p.y > top)
					retval.clipped |= __detail::ClipPTop(p, top);
				retval.visible = true;
			}
			break;

		case 0xA5:
			retval.clipped = __detail::ClipQLeft(p, left);
			if (q.y <= top)
			{
				retval.clipped |= __detail::ClipPRight(p, right);
				if (p.y >= bottom)
				{
					if (q.y < bottom)
						retval.clipped |= __detail::ClipQBottom(p, bottom);
					if (p.y > top)
						retval.clipped |= __detail::ClipPTop(p, top);
					retval.visible = true;
				}
			}
			break;

		case 0xA6:
			break;

		case 0xA8:
			break;

		case 0xA9:
			break;

		case 0xAA:
			break;

		default:
			NP_ENGINE_ASSERT(false, "We arrived at an undefined case(" + to_str((i32)line_code) + ") in ClipLine.");
			break;
		};

		return retval;
	}

	template <typename T>
	static inline ClipLineReturn ClipLine(::glm::vec<2, T>& p, ::glm::vec<2, T>& q, T top, T right)
	{
		return ClipLine<T>(p, top, right, 0, 0);
	}
} // namespace np::alg

#endif /* NP_ENGINE_CLIP_LINE_HPP */
