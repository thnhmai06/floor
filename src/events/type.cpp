#include "floor/events/type.h" // Header

namespace Floor::Events
{
	namespace Event
	{
		namespace Internal
		{
			Event::Event(const Timing::Time& time, const EventType& type)
				: time(time), type(type)
			{
			}

		}
	}
}