//##===----------------------------------------------------------------------===##//
//
//  Author: Nathan Phipps 2/11/26
//
//##===----------------------------------------------------------------------===##//

#ifndef NP_ENGINE_GPU_INTERFACE_MULTISAMPLE_HPP
#define NP_ENGINE_GPU_INTERFACE_MULTISAMPLE_HPP

#include "NP-Engine/Primitive/Primitive.hpp"
#include "NP-Engine/Container/Container.hpp"

namespace np::gpu
{
	class MultisampleUsage : public Enum<ui32>
	{
	protected:
		//store sample count in bits [24, 31]
		constexpr static ui32 SampleCountShift = 24;
		constexpr static ui32 SampleCountMask = BIT(32) - BIT(SampleCountShift);

	public:
		constexpr static ui32 Shading = BIT(0);
		constexpr static ui32 AlphaToCoverage = BIT(1);
		constexpr static ui32 AlphaToOne = BIT(2);

		MultisampleUsage(ui32 value, ui32 sample_count) : Enum<ui32>(value)
		{
			SetSampleCount(sample_count);
		}

		MultisampleUsage(ui32 value) : Enum<ui32>(value) {}

		virtual ui32 GetSampleCount() const
		{
			return GetEmbeddedValue(SampleCountMask, SampleCountShift);
		}

		/*
			only accepting values expressed by eight bits
		*/
		virtual void SetSampleCount(ui32 count)
		{
			SetEmbeddedValue(count, SampleCountMask, SampleCountShift);
		}
	};

	struct Multisample
	{
		MultisampleUsage usage = MultisampleUsage::None;
		dbl minSampleShadingPercentage = 0;
		con::vector<siz> sampleMasks{}; //TODO: investigate more into this (pg 354)
	};
} // namespace np::gpu

#endif /* NP_ENGINE_GPU_INTERFACE_MULTISAMPLE_HPP */