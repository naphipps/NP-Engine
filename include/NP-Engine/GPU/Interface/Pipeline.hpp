//##===----------------------------------------------------------------------===##//
//
//  Author: Nathan Phipps 11/19/21
//
//##===----------------------------------------------------------------------===##//

#ifndef NP_ENGINE_GPU_INTERFACE_PIPELINE_HPP
#define NP_ENGINE_GPU_INTERFACE_PIPELINE_HPP

#include "NP-Engine/Memory/Memory.hpp"
#include "NP-Engine/Primitive/Primitive.hpp"
#include "NP-Engine/Container/Container.hpp"

#include "Detail.hpp"
#include "Device.hpp"
#include "PushData.hpp"
#include "Resource.hpp"

namespace np::gpu
{
	enum class PipelineType : ui32
	{
		None = 0,
		Compute,
		Graphics
	};

	struct PipelineResourceLayout : public DetailObject
	{
		static mem::sptr<PipelineResourceLayout> Create(mem::sptr<Device> device,
														const con::vector<mem::sptr<ResourceLayout>>& resource_layouts,
														PushData push_data);

		virtual ~PipelineResourceLayout() = default;

		virtual mem::sptr<Device> GetDevice() const = 0;

		virtual con::vector<mem::sptr<ResourceLayout>> GetResourceLayouts() const = 0;

		virtual PushData GetPushData() const = 0;

		virtual bl SetPushDataEntry(siz index, const PushDataEntry& entry) = 0;

		//virtual bl SetResourceGroup(siz index, mem::sptr<ResourceGroup> group) = 0; //TODO: do we want this?
	};

	class PipelineUsage : public Enum<ui32>
	{
	public:
		constexpr static ui32 Compute = BIT(0);
		constexpr static ui32 Graphics = BIT(1);

		// TODO: we may not need some of the following... investigate

		constexpr static ui32 Input = BIT(23);
		constexpr static ui32 Tessellation = BIT(24);
		constexpr static ui32 Viewport = BIT(25);
		constexpr static ui32 Rasterization = BIT(26);
		constexpr static ui32 Multisample = BIT(27);
		constexpr static ui32 ColorBlend = BIT(28);
		constexpr static ui32 DepthStencil = BIT(29);
		constexpr static ui32 Dynamic = BIT(30);
		constexpr static ui32 QuickStartup = BIT(31);

		//constexpr static ui32 RayTrace = BIT(2); //TODO: investigate

		PipelineUsage(ui32 value): Enum<ui32>(value) {}
	};

	struct Pipeline : public DetailObject
	{
		virtual ~Pipeline() = default;

		virtual PipelineType GetPipelineType() const = 0;

		virtual mem::sptr<PipelineResourceLayout> GetPipelineResourceLayout() const = 0;

		virtual mem::sptr<Device> GetDevice() const = 0;
	};
} // namespace np::gpu

#endif /* NP_ENGINE_GPU_INTERFACE_PIPELINE_HPP */