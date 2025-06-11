#pragma once
#include <concepts>
#include <filesystem>
#include <SDL3_mixer/SDL_mixer.h>

#include "floor/exceptions.h"

namespace fs = std::filesystem;

namespace Floor::Audio
{
    using Music = std::shared_ptr<Mix_Music>;
    using Effect = std::shared_ptr<Mix_Chunk>;
    template <typename T>
    concept AudioTypename = std::same_as<T, Music> || std::same_as<T, Effect>;

    template <AudioTypename Audio>
    Audio load(typename Audio::element_type* raw);
    template <AudioTypename Audio>
    Audio load(const fs::path& file_path);

    //! Music
    template <>
    inline Music load(Mix_Music* raw)
    {
        return {raw, Mix_FreeMusic};
    }

    template <>
    inline Music load(const fs::path& file_path)
    {
        const auto raw = Mix_LoadMUS(file_path.string().c_str());
        if (!raw) throw Exceptions::SDL_Exception();
        return load<Music>(raw);
    }

    //! Effect
    template <>
    inline Effect load(Mix_Chunk* raw)
    {
        return {raw, Mix_FreeChunk};
    }

    template <>
    inline Effect load(const fs::path& file_path)
    {
        const auto raw = Mix_LoadWAV(file_path.string().c_str());
        if (!raw) throw Exceptions::SDL_Exception();
        return load<Effect>(raw);
    }
}
