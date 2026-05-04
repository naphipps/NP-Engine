//##===----------------------------------------------------------------------===##//
//
//  Author: Nathan Phipps 5/3/23
//
//##===----------------------------------------------------------------------===##//

#ifndef NP_ENGINE_CON_ENTITY_HPP
#define NP_ENGINE_CON_ENTITY_HPP

#include "NP-Engine/Foundation/Foundation.hpp"
#include "NP-Engine/Primitive/Primitive.hpp"
#include "NP-Engine/Memory/Memory.hpp"

#include "NP-Engine/Vendor/EnttInclude.hpp"

namespace np::con
{
	using entity_registry = ::entt::basic_registry<::entt::entity, mem::std_allocator<::entt::entity>>;

	class entity
	{
	private:
		::entt::entity _entity;
		mem::sptr<entity_registry> _registry;

	public:
		entity(mem::sptr<entity_registry> registry): _entity(registry->create()), _registry(registry) {}

		~entity()
		{
			_registry->destroy(_entity);
			_entity = ::entt::null;
		}

		operator ::entt::entity() const
		{
			return _entity;
		}

		mem::sptr<entity_registry> get_registry() const
		{
			return _registry;
		}

		template <typename... T>
		bl has_any() const
		{
			return _registry->any_of<T...>(_entity);
		}

		template <typename... T>
		bl has_all() const
		{
			return _registry->all_of<T...>(_entity);
		}

		template <typename T>
		T& get()
		{
			NP_ENGINE_ASSERT(has_all<T>(), "Cannot get a component we do not have.");
			return _registry->get<T>(_entity);
		}

		template <typename T, typename... Args>
		T& add(Args&&... args)
		{
			NP_ENGINE_ASSERT(!has_all<T>(), "Cannot add a component we already have.");
			return _registry->emplace<T>(_entity, ::std::forward<Args>(args)...);
		}
	};
} // namespace np::con

#endif /* NP_ENGINE_CON_ENTITY_HPP */