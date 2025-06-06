#pragma once
#include <string>
#include <unordered_map>
#include <filesystem>
#include <unordered_set>
#include "floor/audio/type.h"

namespace Floor::Audio::Memory
{
    template <AudioTypename AudioType>
    struct Memory;

    template <AudioTypename AudioType>
    struct Item;

    template <>
    struct Memory<Music>
    {
        using Container = std::unordered_map<std::string, Music>;

        Container items;

        [[nodiscard]] Item<Music> find(const std::string& name) const;
        Item<Music> load(const std::filesystem::path& file_path, const std::string& name, bool override = true);
        void free(const std::string& name);
        void free(const Item<Music>& item);
        void free();

        ~Memory();
    };

    template <>
    struct Memory<Effect>
    {
        using Container = std::unordered_map<std::string, Effect>;

        Container items;

        [[nodiscard]] Item<Effect> find(const std::string& name) const;
        Item<Effect> load(const std::filesystem::path& file_path, const std::string& name, bool override = true);
        void load(
            const std::filesystem::path& folder_path,
            const std::filesystem::path& root_folder_path,
            bool no_except = true, bool recursive = true,
            bool name_has_extension = false, bool name_is_filename = true,
            bool override = true,
            const std::unordered_set<std::string>& only_extensions = {},
            const std::unordered_set<std::filesystem::path>& blacklist = {});
        void free(const std::string& name);
        void free(Item<Effect>& item);
        void free();

        ~Memory();
    };

    template <AudioTypename AudioType>
    struct Item
    {
        const Memory<AudioType>* parent = nullptr;
        typename Memory<AudioType>::Container::const_iterator itr{};

        [[nodiscard]] bool is_valid() const
        {
            return (parent && itr != parent->items.end() && itr->second);
        }

        void reset()
        {
            itr = Memory<AudioType>::Container::const_iterator();
            parent = nullptr;
        };

        Item() = default;
        Item(const Memory<AudioType>* parent, typename Memory<AudioType>::Container::const_iterator item)
            : parent(parent), itr(std::move(item))
        {
        }
    };
}
