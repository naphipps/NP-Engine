//##===----------------------------------------------------------------------===##//
//
//  Author: Nathan Phipps 12/19/22
//
//##===----------------------------------------------------------------------===##//

#ifndef NP_ENGINE_GPU_INTERFACE_RESOURCE_HPP
#define NP_ENGINE_GPU_INTERFACE_RESOURCE_HPP

#include "NP-Engine/Primitive/Primitive.hpp"
#include "NP-Engine/Container/Container.hpp"

#include "Stage.hpp"
#include "Device.hpp"
#include "Detail.hpp"

namespace np::gpu
{
	class ResourceType : public enm_ui32
	{
	public:
		constexpr static ui32 View = BIT(0);
		constexpr static ui32 Buffer = BIT(1); //texel, uniform, storage, dynamic
		constexpr static ui32 Image = BIT(2); //sampled, storage, input, combined-sampler?
		constexpr static ui32 Sampler = BIT(3); //combined-image?

		ResourceType(ui32 value): enm_ui32(value) {}
	};

	class ResourceUsage : public enm_ui32
	{
	public:
		ResourceUsage(ui32 value): enm_ui32(value) {}
	};

	struct Resource : public DetailObject
	{
		virtual ~Resource() = default;

		virtual ResourceType GetResourceType() const = 0;

		virtual bl ClearCacheForDevice(siz offset, siz size)
		{
			return false;
		}

		virtual bl ClearCacheForHost(siz offset, siz size)
		{
			return false;
		}
	};

	struct ResourceView : public Resource
	{
		virtual ~ResourceView() = default;
	};

	class ResourceOperation : public enm_ui32
	{
	public:
		constexpr static ui32 Load = BIT(0);
		constexpr static ui32 Store = BIT(1);

		constexpr static ui32 Clear = BIT(2);
		constexpr static ui32 DontCare = BIT(3);

		ResourceOperation(ui32 value): enm_ui32(value) {}
	};

	struct ResourceDescription
	{
		ResourceType type = ResourceType::None;
		ResourceUsage usage = ResourceUsage::None;
		siz count = 0;
		Stage stage = Stage::None;
		//con::vector<mem::sptr<Sampler>> samplers{}; //TODO: add sampler support -- pretty sure length MUST equal count value
		//or be empty

		bl IsCompatible(const ResourceDescription& other) const
		{
			return type == other.type && usage == other.usage && count == other.count && stage == other.stage;
		}
	};

	struct ResourceLayout : public DetailObject
	{
		static mem::sptr<ResourceLayout> Create(mem::sptr<Device> device, con::vector<ResourceDescription> descriptions);

		virtual ~ResourceLayout() = default;

		virtual mem::sptr<Device> GetDevice() const = 0;

		virtual con::vector<ResourceDescription> GetResourceDescriptions() const = 0;

		virtual bl IsCompatible(mem::sptr<ResourceLayout> other) const
		{
			return false;
		}
	};
} // namespace np::gpu

#endif /* NP_ENGINE_GPU_INTERFACE_RESOURCE_HPP */