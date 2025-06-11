#include "floor/render/layer.h" // Header

namespace Floor::Render
{
    using namespace Floor::Utilities::Math::FPoint;

    //! Layer
    void Layer::render(const SDL_FPoint& multiple)
    {
        if (!config.is_visible()) return;

        for_each(
            buffer,
            [&](const std::shared_ptr<Object>& object) { object->render(config, multiple); },
            [&](const std::shared_ptr<Collection>& collection) { collection->render(config, multiple); });
    }

    Layer::Layer(const SDL_FPoint& top_left_pos)
        : config(top_left_pos)
    {
    }

    //! TextureLayer
    void TextureLayer::render(const bool change_target_when_finish, const SDL_FPoint& multiple)
    {
        if (!config.is_visible() || buffer.empty()) return;
        const auto old_target = SDL_GetRenderTarget(renderer);

        const auto grid_pixel = grid.has_value() ? (get_size(target) / grid.value()) : SDL_FPoint{1, 1};
        const auto final_multiple = multiple * grid_pixel;

        SDL_SetRenderTarget(renderer, target.get());
        Layer::render(final_multiple);
        if (change_target_when_finish) SDL_SetRenderTarget(old_target);
    }

    void TextureLayer::render(const SDL_FPoint& multiple) { render(true, multiple); }

    void TextureLayer::clear()
    {
        buffer.clear();
        Render::clear(target);
    }

    TextureLayer::TextureLayer(const Texture& texture, const SDL_FPoint& top_left_pos)
        : Layer(top_left_pos), target(texture)
    {
    }
}
