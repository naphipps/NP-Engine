//##===----------------------------------------------------------------------===##//
//
//  Author: Nathan Phipps 6/8/26
//
//##===----------------------------------------------------------------------===##//

#ifndef NP_ENGINE_GPU_INTERFACE_RESOURCE_GROUP_HPP
#define NP_ENGINE_GPU_INTERFACE_RESOURCE_GROUP_HPP

#include "NP-Engine/Primitive/Primitive.hpp"
#include "NP-Engine/Container/Container.hpp"

#include "Resource.hpp"
#include "BufferResource.hpp"
#include "ImageResourceView.hpp"
#include "SamplerResource.hpp"

namespace np::gpu
{
	struct ResourceGroup;

	struct ResourceAssignment
	{
		siz resourceDescriptionIndex = SIZ_MAX;
	};

	struct ImageResourceAssignmentContext
	{
		mem::sptr<ImageResourceView> view = nullptr;
		mem::sptr<SamplerResource> sampler = nullptr;
		ImageResourceUsage usage = ImageResourceUsage::None;
	};

	struct BufferResourceAssignmentContext
	{
		mem::sptr<BufferResource> buffer = nullptr;
		siz offset = 0;
		siz range = 0;
	};

	struct ImageResourceAssignment : public ResourceAssignment
	{
		con::vector<ImageResourceAssignmentContext> contexts{};
	};

	struct BufferResourceAssignment : public ResourceAssignment
	{
		con::vector<BufferResourceAssignmentContext> contexts{};
	};

	struct CopyResourceAssignment
	{
		mem::sptr<ResourceGroup> dstResourceGroup = nullptr;
		siz dstResourceDescriptionIndex = SIZ_MAX;
		mem::sptr<ResourceGroup> srcResourceGroup = nullptr;
		siz srcResourceDescriptionIndex = SIZ_MAX;
		siz resourceCount = 0;
	};

	struct ResourceAssignments
	{
		con::vector<ImageResourceAssignment> imageAssignments{};
		con::vector<BufferResourceAssignment> bufferAssignments{};
		con::vector<CopyResourceAssignment> copyAssignments{};
	};

	struct ResourceGroup : public DetailObject
	{
		virtual ~ResourceGroup() = default;

		virtual mem::sptr<ResourceLayout> GetResourceLayout() const = 0;

		virtual siz GetResourceCount() const = 0;

		virtual Result ApplyResourceAssignments(const ResourceAssignments& assignments) = 0;

		virtual bl IsCompatible(mem::sptr<ResourceGroup> other) const
		{
			return false;
		}

		virtual bl IsCompatible(mem::sptr<ResourceLayout> layout) const
		{
			return false;
		}
	};

	struct ResourceGroupPool : public DetailObject
	{
		static mem::sptr<ResourceGroupPool> Create(mem::sptr<Device> device, siz size,
			const con::vector<ResourceDescription>& descriptions);

		virtual ~ResourceGroupPool() = default;

		virtual mem::sptr<Device> GetDevice() const = 0;

		virtual siz GetSize() const = 0;

		/*
			resources pertaining to the returned ResourceGroup will return to the pool upon sptr-destroyer-object-destruct
		*/
		virtual mem::sptr<ResourceGroup> CreateResourceGroup(mem::sptr<ResourceLayout> layout) = 0;

		/*
			resources pertaining to the returned ResourceGroup will return to the pool upon sptr-destroyer-object-destruct
		*/
		virtual con::vector<mem::sptr<ResourceGroup>> CreateResourceGroups(const con::vector<mem::sptr<ResourceLayout>>& layouts) = 0;

		//TODO: do we want a Reset method?? Vulkan supports it, but I'm not so sure
	};
} // namespace np::gpu

#endif /* NP_ENGINE_GPU_INTERFACE_RESOURCE_HPP */