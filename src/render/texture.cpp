#include "floor/render/texture.h" // Header

#include <SDL3_image/SDL_image.h>

#include "floor/exceptions.h"
#include "floor/utilities.h"

namespace Floor::Render
{
    //! Texture
    Texture load(SDL_Texture* raw)
    {
        return {raw, SDL_DestroyTexture};
    }

    Texture load(fs::path file_path)
    {
        file_path = Utilities::Path::normalize_path(file_path);

        auto raw = IMG_LoadTexture(..., file_path.string().c_str());
        if (!raw)
            throw Exceptions::SDL_Exception();
        return load(raw);
    }

    SDL_FPoint get_size(const Texture& texture)
    {
        SDL_FPoint size;
        if (!SDL_GetTextureSize(texture.get(), &size.x, &size.y))
            throw Exceptions::SDL_Exception();
        return size;
    }
    void clear(const Texture& texture)
    {
        const auto old_target = SDL_GetRenderTarget(renderer);
        SDL_SetRenderTarget(renderer, texture.get());
        SDL_RenderClear(renderer);
        SDL_SetRenderTarget(renderer, old_target);
    }
    
    //! Memory
    std::unordered_set<std::string> load_multiple(
        Memory& target,
        const fs::path& folder_path, const fs::path& root_path,
        const bool no_except, const bool recursive,
        const bool name_has_extension, const bool name_is_filename, const bool override,
        const std::unordered_set<std::string>& only_extensions,
        const std::unordered_set<std::string>& whitelist)
    {
        std::unordered_set<std::string> loaded{};

        if (fs::exists(folder_path) && fs::is_directory(folder_path))
            for (const auto& entry : fs::recursive_directory_iterator(folder_path))
            {
                if (recursive && fs::is_directory(entry))
                {
                    loaded.merge(load_multiple(target, entry, root_path, no_except, recursive,
                                      name_has_extension, name_is_filename, override, whitelist));
                }
                else if (fs::is_regular_file(entry))
                {
                    // Make name
                    auto name_path = entry.path().lexically_relative(root_path);
                    if (const auto extension = entry.path().extension().string();
                        !only_extensions.empty() && !only_extensions.contains(extension))
                        continue;
                    if (name_is_filename)
                        name_path = name_path.filename();
                    if (!name_has_extension)
                        name_path.replace_extension();
                    const auto name = name_path.generic_string();

                    // Add item
                    if (!whitelist.empty() && !whitelist.contains(name)) continue;
                    if (!override && target.contains(name)) continue;
                    try
                    {
                        target.emplace(name, load(entry.path()));
                        loaded.insert(name);
                    }
                    catch (...)
                    {
                        if (!no_except) throw;
                    }
                }
            }
        return loaded;
    }
}
