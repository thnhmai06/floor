#pragma once
#include "floor/render/object.h"

namespace Floor::Render
{
    struct Layer
    {
        Buffer buffer;
        RenderConfig config;

        virtual void render(const SDL_FPoint& multiple = {1, 1});

        explicit Layer(const SDL_FPoint& top_left_pos = {0, 0});
        virtual ~Layer() = default;
    };

    struct TextureLayer : Layer
    {
        Texture target; // access mode là SDL_TEXTUREACCESS_STREAMING
        // Grid sẽ mô phỏng việc đặt một lưới grid (đều nhau) lên texture, khi
        // đó tọa độ và kích cỡ object sẽ đối chiếu trên lưới đó
        std::optional<SDL_FPoint> grid = std::nullopt;

        void render(bool change_target_when_finish, const SDL_FPoint& multiple = {1, 1});
        void render(const SDL_FPoint& multiple = {1, 1}) override;
        void clear();

        explicit TextureLayer(const Texture& texture, const SDL_FPoint& top_left_pos = {0, 0});
    };
}
