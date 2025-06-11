#include "floor/render/object.h" // Header

#include "floor/exceptions.h"

namespace Floor::Render
{
    //! Object
    SDL_FRect Object::get_src_rect() const
    {
        const auto shared = Utilities::Pointer::check_weak(src);
        if (!shared) return {0, 0, 0, 0};

        const auto [w, h] = get_size(shared);
        const auto& [s_x, s_y, s_w, s_h] = config.src_rect;
        return {w * s_x, h * s_y, w * s_w, h * s_h};
    }

    SDL_FRect Object::get_dst_rect(const SDL_FPoint& parent) const
    {
        return config.get_dst_rect(Utilities::Render::get_size_from_rect(get_src_rect()), parent);
    }

    void Object::set_render_size(const SDL_FPoint& size)
    {
        config.set_render_size(size, Utilities::Render::get_size_from_rect(get_src_rect()));
    }

    void Object::set_render_size(const float& value)
    {
        set_render_size({value, value});
    }

    bool Object::render(const RenderConfig& parent, const SDL_FPoint& multiple) const
    {
        if (!config.is_visible() || !parent.is_visible()) return false;
        const auto texture = Utilities::Pointer::check_weak(src);
        if (!texture) return false;

        const auto src_rect = get_src_rect();
        const auto src_size = Utilities::Render::get_size_from_rect(src_rect);
        auto dst_rect = get_dst_rect(parent.render_pos);
        dst_rect.x *= multiple.x; dst_rect.y *= multiple.y; dst_rect.w *= multiple.x; dst_rect.h *= multiple.y;
        const auto dst_size = Utilities::Render::get_size_from_rect(dst_rect);
        const SDL_FPoint origin_point = config.anchor_point.get_anchor_pos(src_size, dst_size);
        const uint8_t alpha = config.alpha.get_value() * parent.alpha.get_percent();

        if (!SDL_SetTextureBlendMode(texture.get(), config.blend_mode)
            || !SDL_SetTextureAlphaMod(texture.get(), alpha)
            || !SDL_SetTextureColorMod(texture.get(), config.color.r, config.color.g, config.color.b))
            throw Exceptions::SDL_Exception();

        if (!SDL_RenderTextureRotated(renderer, texture.get(), &src_rect,
                                      &dst_rect, config.angle, &origin_point, config.flip_mode.get_mode()))
            throw Exceptions::SDL_Exception();
        return true;
    }

    Object::Object(
        const Texture& texture,
        const SDL_FPoint& render_pos,
        const AnchorPoint& anchor_point) :
        src(texture), config(render_pos, anchor_point)
    {
    }

    Object::Object(
        const Texture& texture,
        const SDL_FPoint& render_pos,
        const AnchorPoint::Type& anchor_type)
        : src(texture), config(render_pos, anchor_type, get_size(texture))
    {
    }

    //! Collection
    void Collection::for_in_range(const std::function<void(Container::const_reference item)>& function,
                                  const bool no_duplicate) const
    {
        std::unordered_set<void*> completed{};

        if (range.empty())
        {
            auto itr = data.begin();
            while (itr != data.end())
            {
                const auto next = std::next(itr);
                function(*itr);
                itr = next;
            }
        }
        else
            for (auto& [from, to] : range)
            {
                static constexpr auto get_ptr = [](const Container::value_type& item) -> void*
                {
                    if (std::holds_alternative<std::shared_ptr<Object>>(item))
                        return std::get<std::shared_ptr<Object>>(item).get();
                    if (std::holds_alternative<std::shared_ptr<Collection>>(item))
                        return std::get<std::shared_ptr<Collection>>(item).get();
                    return nullptr;
                };

                auto itr = from;
                while (itr != data.end())
                {
                    const auto next = std::next(itr);
                    const auto ptr = get_ptr(*itr);
                    if (no_duplicate && completed.contains(ptr)) continue;
                    completed.insert(ptr);

                    function(*itr);
                    if (itr == to) break;
                    itr = next;
                }
            }
    }
    void Collection::for_in_range(
        const std::function<void(Container::reference& item)>& function,
        const bool no_duplicate)
    {
        std::unordered_set<void*> completed{};

        if (range.empty())
        {
            auto itr = data.begin();
            while (itr != data.end())
            {
                const auto next = std::next(itr);
                function(*itr);
                itr = next;
            }
        }
        else
            for (auto& [from, to] : range)
            {
                static constexpr auto get_ptr = [](const Container::value_type& item) -> void*
                {
                    if (std::holds_alternative<std::shared_ptr<Object>>(item))
                        return std::get<std::shared_ptr<Object>>(item).get();
                    if (std::holds_alternative<std::shared_ptr<Collection>>(item))
                        return std::get<std::shared_ptr<Collection>>(item).get();
                    return nullptr;
                };

                auto itr = from;
                while (itr != data.end())
                {
                    const auto next = std::next(itr);
                    const auto ptr = get_ptr(*itr);
                    if (no_duplicate && completed.contains(ptr)) continue;
                    completed.insert(ptr);

                    function(*itr);
                    if (itr == to) break;
                    itr = next;
                }
            }
    }

    void Collection::render(const RenderConfig& parent, const SDL_FPoint& multiple) const
    {
        using Utilities::Math::FPoint::operator-;

        const auto final = RenderConfig
        {
            config.render_pos - parent.render_pos,
            config.alpha.get_percent() * parent.alpha.get_percent(),
            config.visible && parent.visible
        };
        if (!final.is_visible()) return;

        for_in_range([&final, &multiple](Container::const_reference item)
        {
            if (std::holds_alternative<std::shared_ptr<Object>>(item))
            {
                const auto& object = std::get<std::shared_ptr<Object>>(item);
                object->render(final, multiple);
            }
            if (std::holds_alternative<std::shared_ptr<Collection>>(item))
            {
                const auto& collection = std::get<std::shared_ptr<Collection>>(item);
                collection->render(final, multiple);
            }
        }, true);
    }

    //! Buffer
    void for_each(
        Buffer& buffer,
        const std::function<void(std::shared_ptr<Object>& object)>& object_function,
        const std::function<void(std::shared_ptr<Collection>& collection)>& collection_function,
        const bool collect_garbage)
    {
        if (buffer.empty()) return;

        auto itr = buffer.begin();
        while (itr != buffer.end())
        {
            if (std::holds_alternative<std::weak_ptr<Object>>(*itr))
            {
                auto& weak = std::get<std::weak_ptr<Object>>(*itr);
                auto shared = Utilities::Pointer::check_weak(weak);
                if (!shared)
                {
                    if (collect_garbage)
                        itr = buffer.erase(itr);
                    else ++itr;
                    continue;
                }
                object_function(shared);
                ++itr;
            }
            else if (std::holds_alternative<std::weak_ptr<Collection>>(*itr))
            {
                auto& weak = std::get<std::weak_ptr<Collection>>(*itr);
                auto shared = Utilities::Pointer::check_weak(weak);
                if (!shared)
                {
                    if (collect_garbage)
                        itr = buffer.erase(itr);
                    else ++itr;
                    continue;
                }
                collection_function(shared);
                ++itr;
            }
        }
    }
}
