#pragma once
#include <SDL3/SDL_surface.h>

#include "floor/structures.h"

namespace Floor::Render
{
    struct Color
    {
        uint8_t r, g, b;

        friend Color& operator=(Color& that, const SDL_Color& sdl_color);
    };

    struct FlipMode
    {
        bool horizontal = false; // ngang
        bool vertical = false; // dọc

        [[nodiscard]] SDL_FlipMode get_mode() const;
    };

    constexpr SDL_FPoint SDL_ANCHOR_POINT = {0, 0}; // TopLeft
    struct AnchorPoint : SDL_FPoint
    {
        enum class Type : std::uint8_t
        {
            TopLeft = 0, Centre = 1, CentreLeft = 2, TopRight = 3,
            BottomCentre = 4, TopCentre = 5, Custom = 6,
            CentreRight = 7, BottomLeft = 8, BottomRight = 9
        };

        [[nodiscard]] static AnchorPoint from_type(const Type& anchor_type, const SDL_FPoint& size);
        [[nodiscard]] SDL_FPoint transform_from(const SDL_FPoint& pos, const AnchorPoint& from_anchor = SDL_ANCHOR_POINT) const;
        [[nodiscard]] SDL_FPoint transform_to(const SDL_FPoint& pos, const AnchorPoint& to_anchor = SDL_ANCHOR_POINT) const;
        [[nodiscard]] SDL_FPoint get_anchor_pos(const SDL_FPoint& src_size, const SDL_FPoint& dst_size) const;

        AnchorPoint(const SDL_FPoint& point); // NOLINT(*-explicit-constructor)
        AnchorPoint(const float& x, const float& y); // NOLINT(*-explicit-constructor)
        explicit AnchorPoint(const Type& anchor_type, const SDL_FPoint& size);
    };

    constexpr uint8_t SDL_MAX_ALPHA = 255;

    struct RenderConfig
    {
        SDL_FPoint render_pos = {0, 0};
        PercentValue<uint8_t> alpha{0, SDL_MAX_ALPHA, 1};
        bool visible = true;

        [[nodiscard]] bool is_visible() const;

        RenderConfig() = default;
        explicit RenderConfig(const SDL_FPoint& render_pos, const double& alpha = 1, bool visible = true);
        virtual ~RenderConfig() = default;
    };

    struct ObjectConfig : RenderConfig
    {
        SDL_FRect src_rect = { 0, 0, 1, 1 }; // %
        AnchorPoint anchor_point = {0, 0}; // src
        SDL_FPoint scale = {1.0f, 1.0f}; // theo %
        double angle = 0; // độ
        Color color = {.r = 255, .g = 255, .b = 255};
        FlipMode flip_mode;
        SDL_BlendMode blend_mode = SDL_BLENDMODE_BLEND;

        [[nodiscard]] SDL_FRect get_dst_rect(const SDL_FPoint& src_size, const SDL_FPoint& parent = {}) const;
        void set_render_size(const SDL_FPoint& size, const SDL_FPoint& src_size);
        void set_render_size(const float& value, const SDL_FPoint& src_size);

        ObjectConfig() = default;
        explicit ObjectConfig(const SDL_FPoint& render_pos, const AnchorPoint& anchor_point = SDL_ANCHOR_POINT);
        explicit ObjectConfig(const SDL_FPoint& render_pos, const AnchorPoint::Type& anchor_type, const SDL_FPoint& size);
    };
}
