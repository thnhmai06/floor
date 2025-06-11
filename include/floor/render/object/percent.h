#pragma once
#include "floor/render/object.h"

namespace Floor::Render::Object
{
	struct PercentObject : Object
	{
		SDL_FPoint render_size_when_full;
		const double* percent;
		bool horizontal = true;
		bool vertical = false;

	protected:
		virtual void update();
		void on_before_render() override;

	public:
		explicit PercentObject(
			const Memory::Item& texture,
			const SDL_FPoint& render_size_when_full,
			const double* percent,
			bool horizontal = true,
			bool vertical = false,
			const AnchorLocation& origin_type = AnchorLocation::Centre,
			const SDL_FPoint& render_pos = { 0, 0 });
		explicit PercentObject(
			const Memory::Item& texture,
			const SDL_FPoint& render_size_when_full,
			const double* percent,
			const AnchorPoint& custom_origin,
			bool horizontal = true,
			bool vertical = false,
			const SDL_FPoint& render_pos = { 0, 0 });
	};

	struct StaticPercentObject : PercentObject
	{
		double percent = 1;

		explicit StaticPercentObject(
			const Memory::Item& texture,
			const SDL_FPoint& render_size_when_full,
			bool horizontal = true,
			bool vertical = false,
			const AnchorLocation& origin_type = AnchorLocation::Centre,
			const SDL_FPoint& render_pos = { 0, 0 });
		explicit StaticPercentObject(
			const Memory::Item& texture,
			const SDL_FPoint& render_size_when_full,
			const AnchorPoint& custom_origin,
			bool horizontal = true,
			bool vertical = false,
			const SDL_FPoint& render_pos = { 0, 0 });
	};
}