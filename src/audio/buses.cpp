#include "floor/audio/buses.h" // Header
#include "floor/audio/mixer.h"
#include "floor/exceptions.h"
#include "floor/utilities.h"

namespace Floor::Audio::Buses
{
    //! Bus<Music>
    // ::Volume
    double Bus<Music>::Volume::set_value(const uint8_t& value)
    {
        Mix_VolumeMusic(value);
        return PercentValue::set_value(value);
    }

    double Bus<Mix_Music*>::Volume::get() const
    {
        return get_percent();
    }

    void Bus<Mix_Music*>::Volume::set(const double& percent)
    {
        set_percent(percent);
    }

    void Bus<Mix_Music*>::Volume::set()
    {
        set(get());
    }

    Bus<Mix_Music*>::Volume::Volume(const double& percent)
        : PercentValue(0, MIX_MAX_VOLUME, percent, true, &Mixer::volume)
    {
    }

    // ::
    void Bus<Music>::play(const MusicMemory::Item& music, const bool pause, const int loops)
    {
        if (!music.is_valid()) return;

        if (has_song()) stop();
        Mix_PlayMusic(music.item->second, loops);
        volume.set(); // chỉ set được volume khi có nhạc đang phát
        if (pause)
        {
            Bus::pause();
            seek(0);
        }
        current = music;
    }

    bool Bus<Music>::has_song() { return Mix_PlayingMusic() != 0; }
    bool Bus<Music>::is_playing() { return has_song() && !Mix_PausedMusic(); }

    const MusicMemory::Item& Bus<Music>::get_current()
    {
        if (!has_song())
            current.reset();
        return current;
    }

    Events::Timing::Time Bus<Music>::get_position()
    {
        if (const auto current = get_current(); current.is_valid())
            return static_cast<Events::Timing::Time>(std::round(Mix_GetMusicPosition(current.item->second) * 1000.0));
        return -1;
    }

    void Bus<Music>::seek(const int64_t& new_pos)
    {
        if (!has_song()) return;
        if (const double new_pos_sec = static_cast<double>(new_pos) / 1000.0; !Mix_SetMusicPosition(new_pos_sec))
            throw Exceptions::SDL_Exception();
    }

    void Bus<Music>::pause() { Mix_PauseMusic(); }
    void Bus<Music>::resume() { Mix_ResumeMusic(); }

    void Bus<Music>::stop()
    {
        Mix_HaltMusic();
        current.reset();
    }

    Bus<Music>::Bus(const double& volume) { this->volume.set(volume); }
    Bus<Music>::~Bus() { stop(); }

    //! Bus<Effects>
    double Bus<Effect>::get_channel_volume(const int& channel) const
    {
        const auto value = Mix_Volume(channel, -1);
        const auto percent = volume.get_percent(value);
        const auto bus_volume = volume.get();

        return sound_volume.get_percent(true, static_cast<uint8_t>(value));
    }

    double Bus<Effect>::set_volume(const double& percent)
    {
        const double previous_volume = get_channel_volume();
        const auto channel_num = Mix_AllocateChannels(-1);
        std::vector<double> volume_percent(channel_num);
        for (int channel = 0; channel < channel_num; ++channel)
            volume_percent[channel] = get_channel_volume(channel);

        volume.percent = std::clamp(percent, 0.0, 1.0);
        for (int channel = 0; channel < channel_num; ++channel)
            set_volume(channel, volume_percent[channel]);
        return previous_volume;
    }

    double Bus<Effect>::set_volume(const EffectMemory::Item& sound, double percent) const
    {
        if (!sound.is_valid()) return -1;
        percent = std::clamp(percent, 0.0, 1.0);

        const auto vl = Mix_VolumeChunk(sound.item->second, sound_volume.get_value(true, percent));
        if (vl < 0)
        {
            LOG_ERROR(Logging::Exceptions::Engine::Audio::SDL_Audio_SetEffectVolume_Failed(sound.item->first, percent));
            return -1;
        }

        return sound_volume.get_percent(true, static_cast<uint8_t>(vl));
    }

    double Bus<Effect>::set_volume(const int& channel, double percent) const
    {
        percent = std::clamp(percent, 0.0, 1.0);

        const auto vl = Mix_Volume(channel, sound_volume.get_value(true, percent));
        if (vl < 0)
        {
            LOG_ERROR(Logging::Exceptions::Engine::Audio::SDL_Audio_SetEffectVolume_Failed(channel, percent));
            return -1;
        }

        return sound_volume.get_percent(true, static_cast<uint8_t>(vl));
    }

    int Bus<Effect>::play(const EffectMemory::Item& sound, const std::optional<double>& volume) const
    {
        if (!sound.is_valid()) return -1;

        const int32_t channel = Mix_PlayChannel(-1, sound.item->second, 0);
        if (channel < 0)
        {
            LOG_ERROR(Logging::Exceptions::Engine::Audio::SDL_Audio_PlayEffect_Failed(sound.item->first));
        }
        else
        {
            set_volume(channel, volume.value_or(1.0));
        }
        return channel;
    }

    void Bus<Mix_Chunk*>::stop(const int& channel)
    {
        Mix_HaltChannel(channel);
    }

    Bus<Effect>::Bus(const double& volume, const int& max_channels)
    {
        Mix_AllocateChannels(max_channels);

        sound_volume.parent = &this->volume;
        set_volume(volume);
    }
}
