//##===----------------------------------------------------------------------===##//
//
//  Author: Nathan Phipps 12/20/25
//
//##===----------------------------------------------------------------------===##//

#ifndef NP_ENGINE_ALIGNMENT_HPP
#define NP_ENGINE_ALIGNMENT_HPP

#include <memory>

#include "NP-Engine/Primitive/Primitive.hpp"

namespace np::mem
{
	/*
		the alignment our allocators will default to
	*/
	constexpr static siz DEFAULT_ALIGNMENT = BIT(3);
	constexpr static siz DEFAULT_ALIGNMENT_MINUS_ONE = (DEFAULT_ALIGNMENT - 1);

	constexpr static siz CalcAlignedSize(const siz size, const siz alignment)
	{
		return ((size + alignment - 1) / alignment) * alignment;
	}
} // namespace np::mem

#endif /* NP_ENGINE_ALIGNMENT_HPP */
