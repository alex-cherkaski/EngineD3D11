#pragma once
#include "PCH.h"
#include "ComponentIdGenerator.h"
#include "ComponentSet.h"
#include "EventManager\EventManager.h"
#include "Macros.h"
#include "System.h"
#include "TagIdGenerator.h"
#include "Types.h"

//--------------------------------------------------------------------------------------------------------------------------------

// Registry request priority.
enum class RequestPriority : unsigned char
{
	Deferred = 0,
	Immediate = 1
};

//--------------------------------------------------------------------------------------------------------------------------------

// Deferred add component request.
class IRegistryRequestAddComponent
{
public:
	virtual ~IRegistryRequestAddComponent() = default;
	virtual void Execute() = 0;
};

template<typename TComponent, typename TRequestHandler>
class RegistryRequestAddComponent final : public IRegistryRequestAddComponent
{
public:
	RegistryRequestAddComponent(Registry* registry, const TRequestHandler requestHandler, Entity entity, const TComponent& component)
		: m_component(component)
		, m_entity(entity)
		, m_registry(registry)
		, m_requestHandler(requestHandler)
	{
	}

	virtual ~RegistryRequestAddComponent() = default;

	void Execute() override { (m_registry->*m_requestHandler)(m_entity, m_component); }

private:
	TComponent m_component;
	Entity m_entity;
	Registry* m_registry;
	TRequestHandler m_requestHandler;
};

//--------------------------------------------------------------------------------------------------------------------------------

// Deferred remove component request.
class IRegistryRequestRemoveComponent
{
public:
	virtual ~IRegistryRequestRemoveComponent() = default;
	virtual void Execute() = 0;
};

template<typename TComponent, typename TRequestHandler>
class RegistryRequestRemoveComponent final : public IRegistryRequestRemoveComponent
{
public:
	RegistryRequestRemoveComponent(Registry* registry, const TRequestHandler requestHandler, Entity entity)
		: m_entity(entity)
		, m_registry(registry)
		, m_requestHandler(requestHandler)
	{
	}

	virtual ~RegistryRequestRemoveComponent() = default;

	void Execute() override { (m_registry->*m_requestHandler)(m_entity); }

private:
	Entity m_entity;
	Registry* m_registry;
	TRequestHandler m_requestHandler;
};

//--------------------------------------------------------------------------------------------------------------------------------

// Deferred add tag request.
class IRegistryRequestTag
{
public:
	virtual ~IRegistryRequestTag() = default;
	virtual void Execute() = 0;
};

template<typename TComponent, typename TRequestHandler>
class RegistryRequestTag final : public IRegistryRequestTag
{
public:
	RegistryRequestTag(Registry* registry, const TRequestHandler requestHandler, Entity entity)
		: m_entity(entity)
		, m_registry(registry)
		, m_requestHandler(requestHandler)
	{
	}

	virtual ~RegistryRequestTag() = default;

	void Execute() override { (m_registry->*m_requestHandler)(m_entity); }

private:
	Entity m_entity;
	Registry* m_registry;
	TRequestHandler m_requestHandler;
};

//--------------------------------------------------------------------------------------------------------------------------------

class Registry final
{
	SINGLETON(Registry);

public:
//--------------------------------------------------------------------------------------------------------------------------------

	void RunSystemsInitialize();
	void RunSystemsUpdate(float delatTime);
	void RunSystemsRender();

	void ProcessPendingEntities();
	void ProcessPendingComponents();
	void ProcessPendingTags();

	void Shutdown();

//--------------------------------------------------------------------------------------------------------------------------------

	Entity CreateEntity();
	void RemoveEntity(Entity entity);

//--------------------------------------------------------------------------------------------------------------------------------

	void ProcessComponentAdditions();
	void ProcessComponnetRemovals();

//--------------------------------------------------------------------------------------------------------------------------------

	void ProcessEntityAdditions();
	void ProcessEntityRemovals();

//--------------------------------------------------------------------------------------------------------------------------------

	void ProcessTagAdditions();
	void ProcessTagRemovals();

//--------------------------------------------------------------------------------------------------------------------------------
	
	template<typename TComponent> void AddComponent(Entity entity, const TComponent& component, RequestPriority priority = RequestPriority::Deferred);
	template<typename TComponent> bool HaveComponent(Entity entity) const;
	template<typename TComponent> const TComponent& GetComponentRead(Entity entity) const;
	template<typename TComponent> TComponent& GetComponentWrite(Entity entity);
	template<typename TComponent> void RemoveComponent(Entity entity, RequestPriority priority = RequestPriority::Deferred);

//--------------------------------------------------------------------------------------------------------------------------------

	template<typename TTag> void AddTag(Entity entity, RequestPriority priority = RequestPriority::Deferred);
	template<typename TTag> bool HaveTag(Entity entity) const;
	template<typename TTag> const std::vector<Entity>& GetEntitiesWithTag();
	template<typename TTag> void RemoveTag(Entity entity, RequestPriority priority = RequestPriority::Deferred);

//--------------------------------------------------------------------------------------------------------------------------------

	template<typename TSystem> void AddSystem();
	const std::vector<std::unique_ptr<ISystem>>& GetSystemsRead() const { return m_systems; }
	std::vector<std::unique_ptr<ISystem>>& GetSystemsWrite() { return m_systems; }

//--------------------------------------------------------------------------------------------------------------------------------

	template<typename TComponent> void HandleAddComponentDeferred(Entity entity, const TComponent& component);
	template<typename TComponent> void HandleAddComponentImmediate(Entity entity, const TComponent& component);

	template<typename TComponent> void HandleRemoveComponentDeferred(Entity entity);
	template<typename TComponent> void HandleRemoveComponenImmediate(Entity entity);

//--------------------------------------------------------------------------------------------------------------------------------

	template<typename TTag> void HandleAddTagDeferred(Entity entity);
	template<typename TTag> void HandleAddTagImmediate(Entity entity);

	template<typename TTag> void HandleRemoveTagDeferred(Entity entity);
	template<typename TTag> void HandleRemoveTagImmediate(Entity entity);

//--------------------------------------------------------------------------------------------------------------------------------

private:
	Entity m_nextEntity = 0;

	std::queue<Entity> m_deletedEntities; // The set of recyclable entities.
	std::vector<std::unique_ptr<IComponentSet>> m_componentSets; // All component sets.

	std::vector<ComponentKey> m_entityComponentKeys; // Set bits indicate which components are currently present on the entity.

	std::unordered_map<TagId, std::vector<Entity>> m_tagToEntityMap; // The set of all entities a tag belongs to.
	std::unordered_map<Entity, std::vector<TagId>> m_entityToTagMap; // The set of all tags an entity has.

	std::vector<std::unique_ptr<ISystem>> m_systems; // The set of all entity updating and rendering systems.

	std::vector<Entity> m_addedEntities; // The set of new entities awaiting addition into existing systems.
	std::vector<Entity> m_removedEntities; // The set of entities awaiting complete removal.

	EventManager& m_eventManager = EventManager::GetInstanceWrite();

	std::vector<std::unique_ptr<IRegistryRequestAddComponent>> m_addComponentRequests; // The set of pending add component to entity requests.
	std::vector<std::unique_ptr<IRegistryRequestRemoveComponent>> m_removeComponentRequests; // The set of pending remove component from entity requests.

	std::vector<std::unique_ptr<IRegistryRequestTag>> m_addTagRequests; // The set of pending add tag to entity requests.
	std::vector<std::unique_ptr<IRegistryRequestTag>> m_removeTagRequests; // The set of pending remove tag from entity requests.

	bool m_isInSystemUpdate = false; // Is the registry in the middle of some system update routine.
	bool m_isInSystemRender = false; // Is the registry in the middle of some system render routine.
};

//--------------------------------------------------------------------------------------------------------------------------------

template<typename TComponent>
inline void Registry::AddComponent(Entity entity, const TComponent& component, RequestPriority priority)
{
	switch (priority)
	{
	case RequestPriority::Deferred:
		HandleAddComponentDeferred<TComponent>(entity, component);
		break;
	case RequestPriority::Immediate:
		HandleAddComponentImmediate<TComponent>(entity, component);
		break;
	default:
		assert(false);
		break;
	}
}

//--------------------------------------------------------------------------------------------------------------------------------

template<typename TComponent>
bool Registry::HaveComponent(Entity entity) const
{
	// Get the component id to index into the component sets array.
	static const ComponentId componentId = ComponentIdGenerator::GetComponentId<TComponent>();

	// Check the component key for presence of the corresponding component id.
	return m_entityComponentKeys[entity].test(componentId);
}

//--------------------------------------------------------------------------------------------------------------------------------

template<typename TComponent>
TComponent& Registry::GetComponentWrite(Entity entity)
{
	// Get the component id to index into the component sets array.
	static const ComponentId componentId = ComponentIdGenerator::GetComponentId<TComponent>();

	// Ensure the entity has the component we are attempting to retrieve.
	assert(m_entityComponentKeys[entity].test(componentId));

	// Get the component for the corresponding entity.
	const std::unique_ptr<IComponentSet>& genericComponent = m_componentSets[componentId];
	ComponentSet<TComponent>* specificComponentSet = static_cast<ComponentSet<TComponent>*>(genericComponent.get());
	return specificComponentSet->GetComponentWrite(entity);
}

template<typename TComponent>
const TComponent& Registry::GetComponentRead(Entity entity) const
{
	// Get the component id to index into the component sets array.
	static const ComponentId componentId = ComponentIdGenerator::GetComponentId<TComponent>();

	// Ensure the entity has the component we are attempting to retrieve.
	assert(m_entityComponentKeys[entity].test(componentId));

	// Get the component for the corresponding entity.
	const std::unique_ptr<IComponentSet>& genericComponent = m_componentSets[componentId];
	ComponentSet<TComponent>* specificComponentSet = static_cast<ComponentSet<TComponent>*>(genericComponent.get());
	return specificComponentSet->GetComponentRead(entity);
}

//--------------------------------------------------------------------------------------------------------------------------------

template<typename TComponent>
void Registry::RemoveComponent(Entity entity, RequestPriority priority)
{
	switch (priority)
	{
	case RequestPriority::Deferred:
		HandleRemoveComponentDeferred<TComponent>(entity);
		break;
	case RequestPriority::Immediate:
		HandleRemoveComponenImmediate<TComponent>(entity);
		break;
	default:
		assert(false);
		break;
	}
}

//--------------------------------------------------------------------------------------------------------------------------------

template<typename TTag>
void Registry::AddTag(Entity entity, RequestPriority priority)
{
	switch (priority)
	{
	case RequestPriority::Deferred:
		HandleAddTagDeferred<TTag>(entity);
		break;
	case RequestPriority::Immediate:
		HandleAddTagImmediate<TTag>(entity);
		break;
	default:
		assert(false);
		break;
	}
}

//--------------------------------------------------------------------------------------------------------------------------------

template<typename TTag>
bool Registry::HaveTag(Entity entity) const
{
	bool foundTag = false;

	// Search for an entry of this entity in the entity-tag map.
	const auto iterator = m_entityToTagMap.find(entity);
	if (iterator != m_entityToTagMap.end())
	{
		// Get the entity tag list.
		const std::vector<TagId>& entityTagIds = iterator->second;

		// Get the tag id.
		static const TagId tagId = TagIdGenerator::GetTagId<TTag>();

		// Search for the corresponding tag id in the entity's tag id list.
		foundTag = std::find(entityTagIds.begin(), entityTagIds.end(), tagId) != entityTagIds.end();
	}

	return foundTag;
}

//--------------------------------------------------------------------------------------------------------------------------------

template<typename TTag>
inline const std::vector<Entity>& Registry::GetEntitiesWithTag()
{
	// Get the tag id.
	static const TagId tagId = TagIdGenerator::GetTagId<TTag>();

	// Return the list of entities with this tag, even if its empty.
	return m_tagToEntityMap[tagId];
}

//--------------------------------------------------------------------------------------------------------------------------------

template<typename TTag>
void Registry::RemoveTag(Entity entity, RequestPriority priority)
{
	switch (priority)
	{
	case RequestPriority::Deferred:
		HandleRemoveTagDeferred<TTag>(entity);
		break;
	case RequestPriority::Immediate:
		HandleRemoveTagImmediate<TTag>(entity);
		break;
	default:
		assert(false);
		break;
	}
}

//--------------------------------------------------------------------------------------------------------------------------------

template<typename TSystem>
void Registry::AddSystem()
{
	auto system = new TSystem(*this);
	std::unique_ptr<ISystem> genericSystem(static_cast<ISystem*>(system));
	m_systems.push_back(std::move(genericSystem));
}

template<typename TComponent>
inline void Registry::HandleAddComponentDeferred(Entity entity, const TComponent& component)
{
// Initially allocating a component on the heap my cause it to not be byte aligned as declared,
// but this is temporary as the component will be placed in the appropriate array where they will reside byte aligned.
#pragma warning(disable : 4316)
	// Allocate the component addition request.
	RegistryRequestAddComponent<TComponent, decltype(&Registry::HandleAddComponentImmediate<TComponent>)>* newRegistryRequest
		= new RegistryRequestAddComponent<TComponent, decltype(&Registry::HandleAddComponentImmediate<TComponent>)>(this, &Registry::HandleAddComponentImmediate<TComponent>, entity, component);
#pragma warning(default : 4316)

	// Wrap the request in a unique pointer, and add it to the set of pending component additions.
	std::unique_ptr<IRegistryRequestAddComponent> genericRequest(static_cast<IRegistryRequestAddComponent*>(newRegistryRequest));
	m_addComponentRequests.push_back(std::move(genericRequest));
}

template<typename TComponent>
inline void Registry::HandleAddComponentImmediate(Entity entity, const TComponent& component)
{
	// We cannot add new components when in the middle of a system update or render routine.
	assert(!m_isInSystemUpdate && !m_isInSystemRender);

	// Get the component id to index into the component sets array.
	static const ComponentId componentId = ComponentIdGenerator::GetComponentId<TComponent>();

	// Make room for the new component set if necessary.
	if (componentId >= m_componentSets.size())
	{
		m_componentSets.resize(componentId * 2 + 1);
	}

	// If there is no component set for this component, create one.
	if (m_componentSets[componentId] == nullptr)
	{
		ComponentSet<TComponent>* specificComponentSet = new ComponentSet<TComponent>();
		std::unique_ptr<IComponentSet> setPointer(static_cast<IComponentSet*>(specificComponentSet));
		m_componentSets[componentId] = std::move(setPointer);
	}

	// Add the component to the proper set.
	std::unique_ptr<IComponentSet>& genericComponentSet = m_componentSets[componentId];
	ComponentSet<TComponent>* specificComponentSet = static_cast<ComponentSet<TComponent>*>(genericComponentSet.get());
	specificComponentSet->AddComponent(entity, component);

	// Make room for the entity component key if necessary.
	if (entity >= m_entityComponentKeys.size())
	{
		m_entityComponentKeys.resize(entity * 2 + 1);
	}

	// Mark the component as present in the entity component key.
	m_entityComponentKeys[entity].set(componentId);
}

template<typename TComponent>
inline void Registry::HandleRemoveComponentDeferred(Entity entity)
{
	// Allocate the component removal request.
	RegistryRequestRemoveComponent<TComponent, decltype(&Registry::HandleRemoveComponenImmediate<TComponent>)>* newRegistryRequest
		= new RegistryRequestRemoveComponent<TComponent, decltype(&Registry::HandleRemoveComponenImmediate<TComponent>)>(this, &Registry::HandleRemoveComponenImmediate<TComponent>, entity);

	// Wrap the request in a unique pointer, and add it to the set of pending component removals.
	std::unique_ptr<IRegistryRequestRemoveComponent> genericRequest(static_cast<IRegistryRequestRemoveComponent*>(newRegistryRequest));
	m_removeComponentRequests.push_back(std::move(genericRequest));
}

template<typename TComponent>
inline void Registry::HandleRemoveComponenImmediate(Entity entity)
{
	// We cannot remove components when in the middle of a system update or render routine.
	assert(!m_isInSystemUpdate && !m_isInSystemRender);

	bool canDeleteEntity = false;

	// Get the component id to index into the component sets array.
	static const ComponentId componentId = ComponentIdGenerator::GetComponentId<TComponent>();

	// Remove the component for the corresponding entity, if it exists.
	m_componentSets[componentId]->RemoveComponent(entity);

	// Reset the flag for this component in the entity component key set.
	ComponentKey& componentKey = m_entityComponentKeys[entity];
	componentKey.reset(componentId);

	// Check if the entity has any remaining components.
	canDeleteEntity |= m_entityComponentKeys[entity].none();

	// Check if the remaining components for this entity still qualify it for membership in any of the current systems.
	if (!canDeleteEntity)
	{
		// For each system...
		for (std::unique_ptr<ISystem>& genericSystem : m_systems)
		{
			// Get the system component key.
			const ComponentKey& requiredComponents = genericSystem->GetRequiredComponents();

			// If the component key doesn't math the component set required by the system, remove the entity from the system.
			const bool canRemoveFromSystem = (componentKey | requiredComponents) != requiredComponents;
			if (canRemoveFromSystem)
			{
				genericSystem->RemoveEntity(entity);
			}

			// Update the can delete flag.
			canDeleteEntity |= !canRemoveFromSystem;
		}
	}

	// If the entity has no more components, or if it no longer belongs to any system, remove it completely.
	if (canDeleteEntity)
	{
		RemoveEntity(entity);
	}
}

template<typename TTag>
inline void Registry::HandleAddTagDeferred(Entity entity)
{
	// Allocate the tag addition request.
	RegistryRequestTag<TTag, decltype(&Registry::HandleAddTagImmediate<TTag>)>* newRegistryRequest
		= new RegistryRequestTag<TTag, decltype(&Registry::HandleAddTagImmediate<TTag>)>(this, &Registry::HandleAddTagImmediate<TTag>, entity);

	// Wrap the request in a unique pointer, and add it to the set of pending tag additions.
	std::unique_ptr<IRegistryRequestTag> genericRequest(static_cast<IRegistryRequestTag*>(newRegistryRequest));
	m_addTagRequests.push_back(std::move(genericRequest));
}

template<typename TTag>
inline void Registry::HandleAddTagImmediate(Entity entity)
{
	// We cannot add new tags when in the middle of a system update or render routine.
	assert(!m_isInSystemUpdate && !m_isInSystemRender);

	// Check if we already happen to have this tag.
	assert(!HaveTag<TTag>(entity));

	// Get the tag id.
	static const TagId tagId = TagIdGenerator::GetTagId<TTag>();

	// Append the entity to the list of entities with this tag.
	m_tagToEntityMap[tagId].push_back(entity);

	// Append the tag id to the list of tags the entity has.
	m_entityToTagMap[entity].push_back(tagId);
}

template<typename TTag>
inline void Registry::HandleRemoveTagDeferred(Entity entity)
{
	// Allocate the tag addition request.
	RegistryRequestTag<TTag, decltype(&Registry::HandleRemoveTagImmediate<TTag>)>* newRegistryRequest
		= new RegistryRequestTag<TTag, decltype(&Registry::HandleRemoveTagImmediate<TTag>)>(this, &Registry::HandleRemoveTagImmediate<TTag>, entity);

	// Wrap the request in a unique pointer, and add it to the set of pending tag additions.
	std::unique_ptr<IRegistryRequestTag> genericRequest(static_cast<IRegistryRequestTag*>(newRegistryRequest));
	m_removeTagRequests.push_back(std::move(genericRequest));
}

template<typename TTag>
inline void Registry::HandleRemoveTagImmediate(Entity entity)
{
	// We cannot remove tags when in the middle of a system update or render routine.
	assert(!m_isInSystemUpdate && !m_isInSystemRender);

	// Check if we have the tag we are removing in the first place.
	assert(HaveTag<TTag>(entity));

	// Get the tag id.
	static const TagId tagId = TagIdGenerator::GetTagId<TTag>();

	// Remove the tag entry from the tag-entity map.
	const auto entitySetIterator = m_tagToEntityMap.find(tagId);
	if (entitySetIterator != m_tagToEntityMap.end())
	{
		// Get the set of entities tagged with this tag.
		std::vector<Entity>& taggedEntities = entitySetIterator->second;

		// Find the entity that we want to erase.
		const auto entityIterator = std::find(taggedEntities.begin(), taggedEntities.end(), entity);

		// Erase the entity from the set of entities tagged with this tag.
		taggedEntities.erase(entityIterator);
	}

	// Remove the tag entry from the entity-tag map.
	const auto tagSetIterator = m_entityToTagMap.find(entity);
	if (tagSetIterator != m_entityToTagMap.end())
	{
		// Get the set of tags that this entity has.
		const std::vector<TagId>& entityTags = tagSetIterator->second;

		// Find the tag that we want to erase.
		const auto tagIterator = std::find(entityTags.begin(), entityTags.end(), tagId);

		// Erase the tag from the set of tags this entity has.
		m_entityToTagMap.erase(tagIterator);
	}

	// We should always find the tag in either both sets or neither.
	assert((entitySetIterator == m_tagToEntityMap.find(tagId) && tagSetIterator == m_entityToTagMap.find(entity)) ||
		(entitySetIterator != m_tagToEntityMap.find(tagId) && tagSetIterator != m_entityToTagMap.find(entity)));
}

//--------------------------------------------------------------------------------------------------------------------------------

