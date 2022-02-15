//##===----------------------------------------------------------------------===##//
//
//  Author: Nathan Phipps 2/14/22
//
//##===----------------------------------------------------------------------===##//

#ifndef NP_ENGINE_ENTITY_HPP
#define NP_ENGINE_ENTITY_HPP

#include <utility>

#include "NP-Engine/Primitive/Primitive.hpp"
#include "NP-Engine/Insight/Insight.hpp"

#include "NP-Engine/Vendor/EnttInclude.hpp"

namespace np::ecs
{
	class Entity
	{
	private:
		::entt::entity _entity;
		::entt::registry* _registry;

	public:
		Entity(): _entity(::entt::null), _registry(nullptr) {}

		Entity(::entt::registry& registry): _entity(registry.create()), _registry(memory::AddressOf(registry)) {}

		Entity(::entt::entity entity, ::entt::registry& registry): _entity(entity), _registry(memory::AddressOf(registry)) {}

		Entity(const Entity& other): Entity(other._entity, *other._registry) {}

		Entity(Entity&& other): _entity(::std::move(other._entity)), _registry(::std::move(other._registry)) {}

		~Entity()
		{
			Invalidate();
		}

		Entity& operator=(const Entity& other)
		{
			_entity = other._entity;
			_registry = other._registry;
			return *this;
		}

		Entity& operator=(Entity&& other)
		{
			_entity = ::std::move(other._entity);
			_registry = ::std::move(other._registry);
			return *this;
		}

		bl IsValid() const
		{
			return _entity != ::entt::null && _registry != nullptr && _registry->valid(_entity);
		}

		void Invalidate()
		{
			if (IsValid())
			{
				_registry->destroy(_entity);
				_entity = ::entt::null;
				_registry = nullptr;
			}
		}

		operator bl() const
		{
			return IsValid();
		}

		operator ::entt::entity() const
		{
			return _entity;
		}

		operator ui32() const
		{
			return (ui32)_entity;
		}

		void Clear()
		{
			if (IsValid())
			{
				_registry->destroy(_entity);
				_entity = _registry->create(_entity);
			}
		}

		template <typename... T>
		bl Has() const
		{
			return IsValid() && _registry->all_of<T...>(_entity);
		}

		template <typename T>
		T& Get()
		{
			NP_ASSERT(Has<T>(), "Cannot get a component we do not have.");
			return _registry->get<T>(_entity);
		}

		template <typename T, typename... Args>
		T& Add(Args&&... args)
		{
			NP_ASSERT(Has<T>(), "Cannot add a component we already have.");
			return _registry->emplace<T>(_entity, ::std::forward<Args>(args)...);
		}
	};
} // namespace np::ecs

#endif /* NP_ENGINE_ENTITY_HPP */
