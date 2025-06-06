#include "floor/audio/mixer.h" // Header

#include "floor/exceptions.h"
#include "floor/utilities.h"

namespace Floor::Audio
{
    void Mixer::stop_all()
    {
        music.stop();
        Buses::Bus<Effect>::stop();
    }
    void Mixer::init(const MIX_InitFlags& flags, const SDL_AudioSpec& spec,
                 const double& master_volume, const double& music_volume,
                 const double& effect_volume, const int32_t& max_channels)
    {
        Mix_Init(flags);
        if (!Mix_OpenAudio(0, &spec))
            throw Exceptions::SDL_Exception();

        set_volume(master_volume);
        music.set_volume(music_volume);
        effects.set_volume(effect_volume);
    }

    void Mixer::quit()
    {
        Mix_Quit();
    }
    Mixer::~Mixer() { quit(); }
}
