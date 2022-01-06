#include "GameEventHandler.h"

//---------- FGameEventListeners ----------
bool FGameEventListeners::Dispatch(const FGameEventData& EventData)
{
	if (!GetListener().IsBound())
	{
		return false;
	}

	GetListener().Broadcast(EventData);
	return true;
}

void FGameEventListeners::Clear()
{
#ifdef UE_EDITOR
	ListenersOnDS.Clear();
#endif

	Listeners.Clear();
	HandleMap.Empty();
}

//---------- FGameEventHandler ----------
bool FGameEventHandler::Register(
	const FGameEventType& InEventID,
	const FGameEventStaticDelegate InStaticCallback)
{
	auto ListenersPtr = EventMap.FindOrAdd(InEventID);
	ListenersPtr.Init(InEventID);
	
	if (!ListenersPtr.Register(InStaticCallback))
	{
		return false;
	}

	EventMap[InEventID] = ListenersPtr;
	return true;	
}

bool FGameEventHandler::Unregister(
	const FGameEventType& InEventID,
	const FGameEventStaticDelegate InStaticCallback)
{
	auto ListenersPtr = EventMap.Find(InEventID);
	if (!ListenersPtr)
	{
		return false;
	}
	
	return ListenersPtr->Unregister(InStaticCallback);
}

bool FGameEventHandler::Dispatch(const FGameEventData& EventData)
{	
	const auto ListenersPtr = EventMap.Find(EventData.EventID);
	if (!ListenersPtr)
	{
		return false;
	}

	return ListenersPtr->Dispatch(EventData);
}

void FGameEventHandler::Clear()
{
	for (auto Listeners : EventMap)
	{
		Listeners.Value.Clear();
	}
	
	EventMap.Empty();
}