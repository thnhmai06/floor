#include "floor/events/action/audio.h" // Header

namespace Floor::Events::Action
{
	namespace Audio
	{
		std::shared_ptr<Action> PlayEffectAction::clone() const
		{
			return std::make_shared<PlayEffectAction>(*this);
		}
		void PlayEffectAction::execute(const int64_t& current_time)
		{
			Action::execute(current_time);
			target->play(sound, volume);
			finished = true;
		}
		PlayEffectAction::PlayEffectAction(const Buses::Bus<Effect>& target, const Timing::Time& time,
			Memory::Item<Effect> sound, const std::optional<double> volume)
		: target(&target), sound(std::move(sound)), volume(volume)
		{
			start_time = end_time = time;
		}
	}
}