#include "floor/audio/mixer.h" // Header
#include "floor/exceptions.h"

namespace Floor::Audio
{
    //! Mixer
    // ::Volume
    double Mixer::Volume::set_value(const uint8_t& value)
    {
        const auto percent = PercentValue::set_value(value);

        // re-update
        music->volume.set();
        effects->volume.set();

        return percent;
    }

    double Mixer::Volume::get() const { return get_percent(); }

    void Mixer::Volume::set(const double& percent) { set_percent(percent); }

    Mixer::Volume::Volume(const double& volume)
        : PercentValue(0, MIX_MAX_VOLUME, volume, true)
    {
    }

    // ::
    void Mixer::stop_all()
    {
        music->stop();
        Buses::Bus<Effect>::stop();
    }

    void Mixer::init(const MIX_InitFlags& flags, const SDL_AudioSpec& spec,
                     const std::tuple<double, double, double>& volumes,
                     const int& max_channels, const SDL_AudioDeviceID& device)
    {
        if (initialized) return;
        Mix_Init(flags);
        if (!Mix_OpenAudio(device, &spec))
            throw Exceptions::SDL_Exception();

        const auto& [v_master, v_music, v_effects] = volumes;
        volume = std::make_unique<Volume>(v_master);
        music = std::make_unique<Buses::Bus<Music>>(v_music);
        effects = std::make_unique<Buses::Bus<Effect>>(v_effects, max_channels);

        initialized = true;
    }

    void Mixer::quit()
    {
        music.reset();
        effects.reset();
        volume.reset();

        Mix_CloseAudio();
        Mix_Quit();
        initialized = false;
    }

    Mixer::~Mixer() { quit(); }
}
