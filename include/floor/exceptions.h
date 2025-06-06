#pragma once
#include <stdexcept>
#include <SDL3/SDL_error.h>

namespace Floor::Exceptions
{
    struct SDL_Exception : std::runtime_error
    {
        explicit SDL_Exception() : std::runtime_error(SDL_GetError())
        {
        }
    };
}
