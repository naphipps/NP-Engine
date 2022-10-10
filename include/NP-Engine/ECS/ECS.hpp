//##===----------------------------------------------------------------------===##//
//
//  Author: Nathan Phipps 8/16/20
//
//##===----------------------------------------------------------------------===##//

#ifndef NP_ENGINE_ECS_HPP
#define NP_ENGINE_ECS_HPP

#include <utility>

#include "NP-Engine/Foundation/Foundation.hpp"
#include "NP-Engine/Primitive/Primitive.hpp"
#include "NP-Engine/Memory/Memory.hpp"

#include "NP-Engine/Vendor/EnttInclude.hpp"

namespace np::ecs
{
	using Registry = ::entt::registry;
	using EntityHandle = ::entt::entity;

	class Entity
	{
	private:
		EntityHandle _entity;
		Registry* _registry;

	public:
		template <typename T>
		static T& Get(EntityHandle e, Registry& r)
		{
			return Entity(e, r).get<T>();
		}

		template <typename... T>
		static bl Has(EntityHandle e, Registry& r)
		{
			return Entity(e, r).has<T...>();
		}

		template <typename T, typename... Args>
		static T& Add(EntityHandle e, Registry& r, Args&&... args)
		{
			return Entity(e, r).add<T, Args...>(::std::forward<Args>(args)...);
		}

		Entity(): _entity(::entt::null), _registry(nullptr) {}

		Entity(Registry& registry): _entity(registry.create()), _registry(mem::AddressOf(registry)) {}

		Entity(EntityHandle entity, Registry& registry): _entity(entity), _registry(mem::AddressOf(registry)) {}

		Entity(const Entity& other): _entity(other._entity), _registry(other._registry) {}

		Entity(Entity&& other) noexcept: _entity(::std::move(other._entity)), _registry(::std::move(other._registry))
		{
			other.invalidate();
		}

		~Entity() = default;

		Entity& operator=(const Entity& other)
		{
			_entity = other._entity;
			_registry = other._registry;
			return *this;
		}

		Entity& operator=(Entity&& other) noexcept
		{
			_entity = ::std::move(other._entity);
			_registry = ::std::move(other._registry);
			other.invalidate();
			return *this;
		}

		operator EntityHandle() const
		{
			return _entity;
		}

		Registry& get_registry()
		{
			NP_ENGINE_ASSERT(is_valid(), "Entity must be valid to get registry");
			return *_registry;
		}

		const Registry& get_registry() const
		{
			NP_ENGINE_ASSERT(is_valid(), "Entity must be valid to get registry");
			return *_registry;
		}

		bl is_valid() const
		{
			return _entity != ::entt::null && _registry != nullptr && _registry->valid(_entity);
		}

		void invalidate()
		{
			if (is_valid())
			{
				_entity = ::entt::null;
				_registry = nullptr;
			}
		}

		void clear()
		{
			if (is_valid())
			{
				_registry->destroy(_entity);
				_entity = _registry->create(_entity);
			}
		}

		void destroy()
		{
			if (is_valid())
			{
				_registry->destroy(_entity);
				_entity = ::entt::null;
				_registry = nullptr;
			}
		}

		template <typename... T>
		bl has() const
		{
			return is_valid() && _registry->all_of<T...>(_entity);
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
} // namespace np::ecs

#endif /* NP_ENGINE_ECS_HPP */
