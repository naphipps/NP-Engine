//##===----------------------------------------------------------------------===##//
//
//  Author: Nathan Phipps 3/17/22
//
//##===----------------------------------------------------------------------===##//

#ifndef NP_ENGINE_VULKAN_DESCRIPTOR_SETS_HPP
#define NP_ENGINE_VULKAN_DESCRIPTOR_SETS_HPP

#include "NP-Engine/Vendor/VulkanInclude.hpp"

#include "VulkanDescriptorSetLayout.hpp"
#include "VulkanRenderContext.hpp"

namespace np::gfx::__detail
{
	class VulkanDescriptorSets
	{
	private:
		mem::sptr<RenderContext> _render_context; //TODO: should this be a *Render* Context?? should we go with something more general??
		mem::sptr<VulkanDescriptorSetLayout> _descriptor_set_layout;

		VkDescriptorPool _descriptor_pool;
		con::vector<VkDescriptorSet> _descriptor_sets;

		static VkDescriptorPool CreateDescriptorPool(mem::sptr<RenderContext> context, mem::sptr<VulkanDescriptorSetLayout> layout)
		{
			VulkanRenderContext& render_context = (VulkanRenderContext&)(*context);
			VulkanRenderDevice& render_device = (VulkanRenderDevice&)(*render_context.GetRenderDevice());

			ui32 size = 10; //TODO: where does this value come from??
			con::vector<VkDescriptorPoolSize> pool_sizes;
			for (const VkDescriptorSetLayoutBinding& binding : layout->GetBindings())
				pool_sizes.emplace_back(VkDescriptorPoolSize{binding.descriptorType, size});

			VkDescriptorPoolCreateInfo create_info{};
			create_info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
			create_info.poolSizeCount = (ui32)pool_sizes.size();
			create_info.pPoolSizes = pool_sizes.data();
			create_info.maxSets = render_context.GetFramesInFlightCount();
			create_info.flags = VK_DESCRIPTOR_POOL_CREATE_FREE_DESCRIPTOR_SET_BIT;

			VkDescriptorPool pool = nullptr;
			if (vkCreateDescriptorPool(render_device, &create_info, nullptr, &pool) != VK_SUCCESS)
				pool = nullptr;
			return pool;
		}

		static con::vector<VkDescriptorSet> CreateDescriptorSets(mem::sptr<RenderContext> context, mem::sptr<VulkanDescriptorSetLayout> layout, VkDescriptorPool pool)
		{
			VulkanRenderContext& render_context = (VulkanRenderContext&)(*context);
			VulkanRenderDevice& render_device = (VulkanRenderDevice&)(*render_context.GetRenderDevice());
			con::vector<VkDescriptorSetLayout> layouts(render_context.GetFramesInFlightCount(), *layout);

			VkDescriptorSetAllocateInfo allocate_info{};
			allocate_info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
			allocate_info.descriptorPool = pool;
			allocate_info.descriptorSetCount = (ui32)layouts.size();
			allocate_info.pSetLayouts = layouts.data();

			con::vector<VkDescriptorSet> sets(layouts.size());
			if (vkAllocateDescriptorSets(render_device, &allocate_info, sets.data()) != VK_SUCCESS)
				sets.clear();

			return sets;
		}

	public:
		VulkanDescriptorSets(mem::sptr<RenderContext> render_context, mem::sptr<VulkanDescriptorSetLayout> descriptor_set_layout):
			_render_context(render_context),
			_descriptor_set_layout(descriptor_set_layout),
			_descriptor_pool(CreateDescriptorPool(_render_context, _descriptor_set_layout)),
			_descriptor_sets(CreateDescriptorSets(_render_context, _descriptor_set_layout, _descriptor_pool))
		{}

		~VulkanDescriptorSets()
		{
			if (_descriptor_pool)
			{
				VulkanRenderDevice& render_device = (VulkanRenderDevice&)(*_render_context->GetRenderDevice());
				vkDestroyDescriptorPool(render_device, _descriptor_pool, nullptr);
				_descriptor_pool = nullptr;
			}
		}

		operator const con::vector<VkDescriptorSet>&() const
		{
			return _descriptor_sets;
		}

		VkDescriptorSet operator[](i32 index) const
		{
			return _descriptor_sets[index];
		}

		mem::sptr<RenderContext> GetRenderContext() const
		{
			return _render_context;
		}

		mem::sptr<VulkanDescriptorSetLayout> GetLayout() const
		{
			return _descriptor_set_layout;
		}

		void Rebuild()
		{
			VulkanRenderDevice& render_device = (VulkanRenderDevice&)(*_render_context->GetRenderDevice());
			vkDestroyDescriptorPool(render_device, _descriptor_pool, nullptr);
			_descriptor_pool = CreateDescriptorPool(_render_context, _descriptor_set_layout);
			_descriptor_sets = CreateDescriptorSets(_render_context, _descriptor_set_layout, _descriptor_pool);
		}

		void SubmitWriter(VkWriteDescriptorSet& writer)
		{
			VulkanRenderContext& render_context = (VulkanRenderContext&)(*_render_context);
			VulkanRenderDevice& render_device = (VulkanRenderDevice&)(*render_context.GetRenderDevice());

			_descriptor_set_layout->GetBindingForDescriptorType(writer.descriptorType, writer.dstBinding);
			writer.dstSet = _descriptor_sets[render_context.GetCurrentImageIndex()];
			vkUpdateDescriptorSets(render_device, 1, mem::AddressOf(writer), 0, nullptr);
		}

		void SubmitWriters(con::vector<VkWriteDescriptorSet>& writers)
		{
			VulkanRenderContext& render_context = (VulkanRenderContext&)(*_render_context);
			VulkanRenderDevice& render_device = (VulkanRenderDevice&)(*render_context.GetRenderDevice());

			for (VkWriteDescriptorSet& writer : writers)
			{
				_descriptor_set_layout->GetBindingForDescriptorType(writer.descriptorType, writer.dstBinding);
				writer.dstSet = _descriptor_sets[render_context.GetCurrentImageIndex()];
			}

			vkUpdateDescriptorSets(render_device, (ui32)writers.size(), writers.data(), 0, nullptr);
		}
	};
} // namespace np::gfx::__detail

#endif /* NP_ENGINE_VULKAN_DESCRIPTOR_SETS_HPP */