#pragma once
#include "floor/render/object.h"
#include "floor/events/event.h"

namespace Floor::Render::Object
{
	struct InteractiveObject : Object
	{
		bool allow_when_hide = false;
		const Events::Event::External::Buffer* event_buffer;
		std::shared_ptr<Events::Condition::External::Condition> condition;
		std::function<void(Object* object, const Events::Event::External::Event& event)> on_event;

		virtual void update();

		explicit InteractiveObject(
			Memory::Item texture,
			const Events::Event::External::Buffer& event_buffer,
			std::shared_ptr<Events::Condition::External::Condition> condition,
			const AnchorLocation& origin_type = AnchorLocation::Centre,
			const SDL_FPoint& render_pos = { 0, 0 },
			std::function<void(Object* object, const Events::Event::External::Event& event)> on_event = nullptr);
		explicit InteractiveObject(
			Memory::Item texture,
			const Events::Event::External::Buffer& event_buffer,
			std::shared_ptr<Events::Condition::External::Condition> condition,
			const Config::AnchorPoint& custom_origin,
			const SDL_FPoint& render_pos = { 0, 0 },
			std::function<void(Object* object, const Events::Event::External::Event& event)> on_event = nullptr);
	};
}