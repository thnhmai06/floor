#include "floor/render/config.h" // Header

namespace Floor::Render
{
	//! Color
	Color& operator=(Color& that, const SDL_Color& sdl_color)
	{
		that.r = sdl_color.r;
		that.g = sdl_color.g;
		that.b = sdl_color.b;
		return that;
	}

	//! FlipMode
	SDL_FlipMode FlipMode::get_mode() const
	{
		return static_cast<SDL_FlipMode>((horizontal ? SDL_FLIP_HORIZONTAL : 0) | (vertical ? SDL_FLIP_VERTICAL : 0));
	}

	//! AnchorPoint
	SDL_FPoint AnchorPoint::transform_from(const SDL_FPoint& pos, const AnchorPoint& from_anchor) const
	{
		return {pos.x - from_anchor.x + x, pos.y - from_anchor.y + y};
	}

	SDL_FPoint AnchorPoint::transform_to(const SDL_FPoint& pos, const AnchorPoint& to_anchor) const
	{
		return {pos.x - x + to_anchor.x, pos.y - y + to_anchor.y};
	}

	SDL_FPoint AnchorPoint::get_anchor_pos(const SDL_FPoint& src_size, const SDL_FPoint& dst_size) const
	{
		using Utilities::Math::FPoint::operator*;
		using Utilities::Math::FPoint::operator/;

		const auto scale = dst_size / src_size;
		return *this * scale;
	}

	AnchorPoint AnchorPoint::from_type(
		const Type& anchor_type, const SDL_FPoint& size)
	{
		const auto& [w, h] = size;
		switch (anchor_type)
		{
		case Type::TopLeft:
		case Type::Custom:
			return {0, 0};
		case Type::BottomLeft:
			return {0, h};
		case Type::BottomRight:
			return {w, h};
		case Type::TopRight:
			return {w, 0};
		case Type::TopCentre:
			return {w / 2, 0};
		case Type::BottomCentre:
			return {w / 2, h};
		case Type::CentreLeft:
			return {0, h / 2};
		case Type::CentreRight:
			return {w, h / 2};
		case Type::Centre:
			return {w / 2, h / 2};
		}
		return {0, 0};
	}

	AnchorPoint::AnchorPoint(const SDL_FPoint& point): SDL_FPoint(point)
	{
	}

	AnchorPoint::AnchorPoint(const float& x, const float& y): SDL_FPoint{x, y}
	{
	}

	AnchorPoint::AnchorPoint(
		const Type& anchor_type, const SDL_FPoint& size)
		: AnchorPoint(from_type(anchor_type, size))
	{
	}

	//! RenderConfig
	RenderConfig::RenderConfig(const SDL_FPoint& render_pos, const double& alpha, const bool visible)
		: render_pos(render_pos), visible(visible)
	{
		this->alpha.set_percent(alpha);
	}
	bool RenderConfig::is_visible() const
	{
		return visible && alpha.get_percent() > 0;
	}

	//! ObjectConfig
	void ObjectConfig::set_render_size(const SDL_FPoint& size, const SDL_FPoint& src_size)
	{
		if (src_size.x > 0) scale.x = size.x / src_size.x; else scale.x = 0;
		if (src_size.y > 0) scale.y = size.y / src_size.y; else scale.y = 0;
	}
	void ObjectConfig::set_render_size(const float& value, const SDL_FPoint& src_size)
	{
		set_render_size({ value, value }, src_size);
	}
	SDL_FRect ObjectConfig::get_dst_rect(const SDL_FPoint& src_size, const SDL_FPoint& parent) const
	{
		using Utilities::Math::FPoint::operator*;
		using Utilities::Math::FPoint::operator-;

		const auto pos = anchor_point.transform_to(render_pos);
		const auto size = src_size * scale;
		return Utilities::Render::merge_pos_size(pos - parent, size);
	}
	ObjectConfig::ObjectConfig(const SDL_FPoint& render_pos, const AnchorPoint& anchor_point) :
		RenderConfig(render_pos), anchor_point(anchor_point)
	{
	}

	ObjectConfig::ObjectConfig(const SDL_FPoint& render_pos, const AnchorPoint::Type& anchor_type, const SDL_FPoint& size)
		: ObjectConfig(render_pos, AnchorPoint(anchor_type, size))
	{
	}
}
