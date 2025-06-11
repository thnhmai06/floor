#pragma once
#include <functional>
#include <variant>

#include "floor/render/texture.h"
#include "floor/render/config.h"

namespace Floor::Render
{
	struct Object
	{
		Texture::weak_type src;
		ObjectConfig config;

		[[nodiscard]] SDL_FRect get_src_rect() const;
		[[nodiscard]] SDL_FRect get_dst_rect(const SDL_FPoint& parent = {0, 0}) const;
		void set_render_size(const SDL_FPoint& size);
		void set_render_size(const float& value);
		virtual bool render(const RenderConfig& parent = {}, const SDL_FPoint& multiple = {1, 1}) const;

		Object() = default;
		explicit Object(
			const Texture& texture,
			const SDL_FPoint& render_pos,
			const AnchorPoint::Type& anchor_type = AnchorPoint::Type::Centre);
		explicit Object(
			const Texture& texture,
			const SDL_FPoint& render_pos,
			const AnchorPoint& anchor_point);
		virtual ~Object() = default;
	};

	struct Collection
	{
		using Container = std::list<std::variant<std::shared_ptr<Object>, std::shared_ptr<Collection>>>;

		Container data;
		RenderConfig config;
		std::list<std::pair<Container::iterator, Container::iterator>> range;

		void for_in_range(const std::function<void(Container::const_reference& item)>& function, bool no_duplicate = false) const;
		void for_in_range(const std::function<void(Container::reference& item)>& function, bool no_duplicate = false);
		virtual void render(const RenderConfig& parent = {}, const SDL_FPoint& multiple = {1, 1}) const;

		virtual ~Collection() = default;
	};

	using Buffer = std::list<std::variant<std::weak_ptr<Object>, std::weak_ptr<Collection>>>;

	void for_each(
		Buffer& buffer,
		const std::function<void(std::shared_ptr<Object>& object)>& object_function,
		const std::function<void(std::shared_ptr<Collection>& collection)>& collection_function,
		bool collect_garbage = true);
}