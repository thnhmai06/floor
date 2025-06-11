#pragma once
#include <string>
#include <unordered_map>
#include <filesystem>
#include <unordered_set>

#include "floor/audio/audio.h"

namespace fs = std::filesystem;

namespace Floor::Audio
{
    template <AudioTypename Audio>
    using Memory = std::unordered_map<std::string, Audio>;

    template <AudioTypename Audio>
    std::unordered_set<std::string> load_multiple(
        Memory<Audio>& target,
        const fs::path& folder_path,
        const fs::path& root_path,
        const bool no_except = false, const bool recursive = true,
        const bool name_has_extension = false, const bool name_is_filename = true, const bool override = true,
        const std::unordered_set<std::string>& only_extensions = {},
        const std::unordered_set<std::string>& blacklist = {})
    {
        std::unordered_set<std::string> loaded{};
        if (fs::exists(folder_path) && fs::is_directory(folder_path))
        {
            for (const auto& entry : fs::recursive_directory_iterator(folder_path))
            {
                if (recursive && fs::is_directory(entry))
                {
                    loaded.merge(load_multiple(target, entry, root_path, no_except, recursive,
                                               name_has_extension, name_is_filename, override, only_extensions,
                                               blacklist));
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
                    if (blacklist.contains(name)) continue;
                    if (!override && target.contains(name)) continue;
                    try
                    {
                        target.emplace(name, load<Audio>(entry.path()));
                        loaded.insert(name);
                    }
                    catch (...)
                    {
                        if (!no_except) throw;
                    }
                }
            }
        }
        return loaded;
    }
}
