#pragma once
#include "PCH.h"
#include "Macros.h"
#include "EventIdGenerator.h"

//--------------------------------------------------------------------------------------------------------------------------------

enum class EventPriority : unsigned char
{
	Deferred = 0,
	Immediate = 1
};

//--------------------------------------------------------------------------------------------------------------------------------

class IEventVector
{
public:
	IEventVector() = default;
	virtual ~IEventVector() = default;
	virtual void* GetEvent(size_t index) = 0;
	virtual size_t GetElementCount() const = 0;
	virtual void Clear() = 0;
};

template<typename TEvent>
class EventVector final : public IEventVector
{
public:
	EventVector() = default;
	~EventVector() = default;

	void AddEvent(const TEvent& event);
	void* GetEvent(size_t index) override;
	size_t GetElementCount() const override;
	void Clear() override;

private:
	std::vector<TEvent> m_events;
};

template<typename TEvent>
void EventVector<TEvent>::AddEvent(const TEvent& event)
{
	m_events.push_back(std::move(event));
}

template<typename TEvent>
void* EventVector<TEvent>::GetEvent(size_t index)
{
	assert(index < m_events.size());
	return static_cast<void*>(&m_events[index]);
}

template<typename TEvent>
size_t EventVector<TEvent>::GetElementCount() const
{
	return m_events.size();
}

template<typename TEvent>
void EventVector<TEvent>::Clear()
{
	m_events.clear();
}

//--------------------------------------------------------------------------------------------------------------------------------

class IEventHandler
{
public:
	NO_COPY(IEventHandler);
	NO_MOVE(IEventHandler);

	IEventHandler() = default;
	virtual ~IEventHandler() = default;
	virtual void HandleEvent(const void* event) const = 0;
};

template<typename TEvent, typename TOwner>
class EventHandler final : public IEventHandler
{
public:
	EventHandler(TOwner* owner, void(TOwner::* callback)(const TEvent& event));
	~EventHandler() = default;
	void HandleEvent(const void* event) const override;

private:
	TOwner* m_owner;
	void(TOwner::* m_callback)(const TEvent& event);
};

template<typename TEvent, typename TOwner>
EventHandler<TEvent, TOwner>::EventHandler(TOwner* owner, void(TOwner::* callback)(const TEvent& event))
	: IEventHandler::IEventHandler()
	, m_owner(owner)
	, m_callback(callback)
{
}

template<typename TEvent, typename TOwner>
void EventHandler<TEvent, TOwner>::HandleEvent(const void* event) const
{
	// Cast the incoming event to is true type.
	const TEvent* specificEvent = static_cast<const TEvent*>(event);

	// Invoke the event callback on the event.
	(m_owner->*m_callback)(*specificEvent);
}

//--------------------------------------------------------------------------------------------------------------------------------

class EventManager final
{
	SINGLETON(EventManager);

public:
	template<typename TEvent, typename TOwner> void SubscribeToEvent(TOwner* owner, void(TOwner::*callback)(const TEvent& event));
	template<typename TEvent> void EmitEvent(const TEvent& event, EventPriority priority);

	void Update();
	void Shutdown();

private:
	template<typename TEvent> void HandleDeferredEvent(const TEvent& event);
	template<typename TEvent> void HandleImmediateEvent(const TEvent& event);
	
private:
	std::unordered_map<EventId, std::vector<std::unique_ptr<IEventHandler>>> m_eventHandlerMap;
	std::unordered_map<EventId, std::unique_ptr<IEventVector>> m_pendingEventMap;
};

template<typename TEvent, typename TOwner>
inline void EventManager::SubscribeToEvent(TOwner* owner, void(TOwner::* callback)(const TEvent& event))
{
	// Get the corresponding event Id.
	static const EventId eventId = EventIdGenerator::GetEventId<TEvent>();

	// Allocate the corresponding event handler.
	EventHandler<TEvent, TOwner>* eventHandler = new EventHandler<TEvent, TOwner>(owner, callback);

	// Cast the event handler to its generic version, and wrap it in a smart pointer.
	std::unique_ptr<IEventHandler> genericEventHandler(static_cast<IEventHandler*>(eventHandler));

	// Move the event handler into the current set of event handlers for this event.
	m_eventHandlerMap[eventId].push_back(std::move(genericEventHandler));
}

template<typename TEvent>
void EventManager::EmitEvent(const TEvent& event, EventPriority priority)
{
	// Check the priority of the event and handle it accordingly.
	switch (priority)
	{
	case EventPriority::Deferred:
		HandleDeferredEvent(event);
		break;
	case EventPriority::Immediate:
		HandleImmediateEvent(event);
		break;
	default:
		break;
	}
}

template<typename TEvent>
void EventManager::HandleDeferredEvent(const TEvent& event)
{
	// Get the corresponding event Id.
	static const EventId eventId = EventIdGenerator::GetEventId<TEvent>();

	// If we don't have a corresponding event vector, create one.
	if (m_pendingEventMap.find(eventId) == m_pendingEventMap.end())
	{
		m_pendingEventMap[eventId].reset(static_cast<IEventVector*>(new EventVector<TEvent>()));
	}

	// Get the event vector.
	EventVector<TEvent>* eventVector = static_cast<EventVector<TEvent>*>(m_pendingEventMap[eventId].get());

	// Add the event.
	eventVector->AddEvent(std::move(event));
}

template<typename TEvent>
void EventManager::HandleImmediateEvent(const TEvent& event)
{
	// Get the corresponding event Id.
	static const EventId eventId = EventIdGenerator::GetEventId<TEvent>();

	// If we have a set of event handlers for this event...
	if (m_eventHandlerMap.find(eventId) != m_eventHandlerMap.end())
	{
		// Run the event through all the corresponding event handlers.
		for (std::unique_ptr<IEventHandler>& eventHandler : m_eventHandlerMap[eventId])
		{
			eventHandler->HandleEvent(&event);
		}
	}
}

inline void EventManager::Update()
{
	// For every pending event pair...
	for (std::pair<const EventId, std::unique_ptr<IEventVector>>& pendingEventPair : m_pendingEventMap)
	{
		// Get the event Id and the set of pending events.
		const EventId eventId = pendingEventPair.first;
		std::unique_ptr<IEventVector>& eventVector = pendingEventPair.second;

		// For every event handler for this event type...
		for (const std::unique_ptr<IEventHandler>& eventHandler : m_eventHandlerMap[eventId])
		{
			// Run each pending event of the current type through the event handler.
			for (size_t index = 0; index < eventVector->GetElementCount(); ++index)
			{
				eventHandler->HandleEvent(eventVector->GetEvent(index));
			}
		}

		// Clear the now processed set of events.
		eventVector->Clear();
	}
}

inline void EventManager::Shutdown()
{
	m_eventHandlerMap.clear();
	m_pendingEventMap.clear();
}

//--------------------------------------------------------------------------------------------------------------------------------

