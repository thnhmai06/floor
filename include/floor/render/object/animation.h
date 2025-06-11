#pragma once
#include "floor/render/object.h"
#include "floor/events/timing/time.h"

namespace Floor::Render::Object
{
	using Events::Timing::Timer;

	enum class LoopType : bool
	{
		LoopForever = false,
		LoopOnce = true
	};

	struct AnimationObject : Object
	{
	protected:
		std::optional<int64_t> last_frame_time;
		long long current_frame = 0;

		void update();
		void on_before_render() override;
	public:
		std::weak_ptr<const Timer> timer;
		std::vector<Memory::Item> frames;
		int64_t frame_delay;
		LoopType loop_type;

		explicit AnimationObject(
			std::weak_ptr<const Timer> timer,
			std::vector<Memory::Item> frames, const int64_t& frame_delay,
			const LoopType& loop_type = LoopType::LoopForever,
			const AnchorLocation& origin_type = AnchorLocation::Centre,
			const SDL_FPoint& render_pos = { .x = 0, .y = 0 });
		explicit AnimationObject(
			std::weak_ptr<const Timer> timer,
			std::vector<Memory::Item> frames, const int64_t& frame_delay,
			const Config::AnchorPoint& custom_origin,
			const LoopType& loop_type = LoopType::LoopForever,
			const SDL_FPoint& render_pos = {.x = 0, .y = 0 });
	};
}