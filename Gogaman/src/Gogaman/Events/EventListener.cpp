#include "pch.h"
#include "EventListener.h"
#include "EventQueue.h"

namespace Gogaman
{
	EventListener::EventListener()
	{
		EventQueue::AddListener(*this);
	}
}