//##===----------------------------------------------------------------------===##//
//
//  Author: Nathan Phipps 5/3/23
//
//##===----------------------------------------------------------------------===##//

#ifndef NP_ENGINE_ENTITY_HPP
#define NP_ENGINE_ENTITY_HPP

#include "NP-Engine/Foundation/Foundation.hpp"
#include "NP-Engine/Primitive/Primitive.hpp"
#include "NP-Engine/Memory/Memory.hpp"

#include "NP-Engine/Vendor/EnttInclude.hpp"

namespace np::con
{
	using entity_registry = ::entt::registry;

	class entity
	{
	private:
		::entt::entity _entity;
		mem::sptr<entity_registry> _registry;

	public:
		entity(mem::sptr<entity_registry> registry) : _entity(registry->create()), _registry(registry) {}

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
		bl has() const
		{
			return _registry->all_of<T...>(_entity);
		}

		template <typename T>
		T& get()
		{
			NP_ENGINE_ASSERT(has<T>(), "Cannot get a component we do not have.");
			return _registry->get<T>(_entity);
		}

		template <typename T, typename... Args>
		T& add(Args&&... args)
		{
			NP_ENGINE_ASSERT(!has<T>(), "Cannot add a component we already have.");
			return _registry->emplace<T>(_entity, ::std::forward<Args>(args)...);
		}
	};
}

#endif /* NP_ENGINE_ENTITY_HPP */