#pragma once
#include "floor/audio/type.h"
#include "floor/audio/buses.h"
#include "floor/events/action.h"

namespace Floor::Events::Action
{
	namespace Audio
	{
		using namespace Floor::Audio;

		struct PlayEffectAction : Action
		{
			const Buses::Bus<Effect>* target;
			Memory::EffectMemory::Item sound;
			std::optional<double> volume;

			[[nodiscard]] std::shared_ptr<Action> clone() const override;
			void execute(const int64_t& current_time) override;

			explicit PlayEffectAction(
				const Buses::Bus<Effect>& target, const Timing::Time& time,
				Memory::EffectMemory::Item sound, std::optional<double> volume = std::nullopt);
		};
	}
}