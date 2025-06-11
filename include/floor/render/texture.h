#pragma once
#include <filesystem>
#include <unordered_map>
#include <unordered_set>
#include <SDL3/SDL_render.h>

namespace fs = std::filesystem;

namespace Floor::Render
{
    //! Texture
    using Texture = std::shared_ptr<SDL_Texture>;
    Texture load(SDL_Texture* raw);
    Texture load(fs::path file_path);
    SDL_FPoint get_size(const Texture& texture);
    void clear(const Texture& texture);

    //! Memory
    using Memory = std::unordered_map<std::string, Texture>;
    std::unordered_set<std::string> load_multiple(
            Memory& target,
            const fs::path& folder_path, const fs::path& root_path,
            bool no_except = false, bool recursive = true,
            bool name_has_extension = false, bool name_is_filename = false, bool override = true,
            const std::unordered_set<std::string>& only_extensions = {},
            const std::unordered_set<std::string>& whitelist = {});
}