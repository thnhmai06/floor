#include "floor/audio/buses.h" // Header
#include "floor/audio/mixer.h"
#include "floor/exceptions.h"

namespace Floor::Audio
{
    //! Bus<Music>
    // ::Volume
    double Bus<Music>::Volume::set_value(const uint8_t& value)
    {
        Mix_VolumeMusic(value);
        return PercentValue::set_value(value);
    }

    double Bus<Music>::Volume::get() const { return get_percent(); }
    void Bus<Music>::Volume::set(const double& percent) { set_percent(percent); }
    void Bus<Music>::Volume::set() { set(get()); }

    Bus<Music>::Volume::Volume(const double& percent)
        : PercentValue(0, MIX_MAX_VOLUME, percent, true, Mixer::volume.get())
    {
    }

    // ::
    bool Bus<Music>::play(const Music& music, const bool pause, const int loops)
    {
        if (!music) return false;

        if (has_song()) stop();
        Mix_PlayMusic(music.get(), loops);
        volume.set(); // chỉ set được volume khi có nhạc đang phát
        if (pause)
        {
            Bus::pause();
            seek(0);
        }
        current_music = music;
        return true;
    }

    bool Bus<Music>::has_song() { return Mix_PlayingMusic() != 0; }
    bool Bus<Music>::is_playing() { return has_song() && !Mix_PausedMusic(); }

    const Music::weak_type& Bus<Music>::get_current_music()
    {
        if (!has_song()) current_music = Music::weak_type();
        return current_music;
    }

    Events::Timing::Time Bus<Music>::get_position()
    {
        if (const auto current = Utilities::Pointer::check_weak(get_current_music()))
            return static_cast<Events::Timing::Time>(std::round(Mix_GetMusicPosition(current.get()) * 1000.0));
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
        current_music = Music::weak_type();
    }

    Bus<Music>::Bus(const double& volume) { this->volume.set(volume); }
    Bus<Music>::~Bus() { stop(); }

    //! Bus<Effect>
    // ::Volume
    double Bus<Effect>::Volume::set_value(const uint8_t& value)
    {
        const auto channel_num = Mix_AllocateChannels(-1);
        std::vector crr_channels_percent(channel_num, 1.0);
        for (int channel = 0; channel < channel_num; ++channel)
        {
            const uint8_t crr_channels_value = Mix_Volume(channel, -1);
            crr_channels_percent[channel] = channel_volume.get_percent(crr_channels_value);
        }
        const auto percent = PercentValue::set_value(value);
        for (int channel = 0; channel < channel_num; ++channel)
            Mix_Volume(channel, channel_volume.get_value(crr_channels_percent[channel]));

        return percent;
    }
    double Bus<Effect>::Volume::get(const int& channel) const
    {
        if (channel < 0)
            return get_percent();
        const auto value = Mix_Volume(channel, -1);
        return channel_volume.get_percent(value);
    }

    void Bus<Effect>::Volume::set(const double& percent, const int& channel)
    {
        if (channel < 0)
            set_percent(percent);
        else
        {
            const auto new_value = channel_volume.get_value(percent);
            Mix_Volume(channel, new_value);
        }
    }

    void Bus<Effect>::Volume::set() { set(get()); }

    Bus<Effect>::Volume::Volume(const double& percent)
        : PercentValue(0, MIX_MAX_VOLUME, percent, true, Mixer::volume.get())
    {
    }

    // ::
    int Bus<Effect>::play(
        const Effect& sound,
        const double& volume, const int& loops, const int& channel)
    {
        if (!sound) return -1;

        const auto out_channel = Mix_PlayChannel(channel, sound.get(), loops);
        if (out_channel < 0)
            throw Exceptions::SDL_Exception();
        this->volume.set(volume, out_channel);
        return out_channel;
    }

    void Bus<Effect>::stop(const int& channel)
    {
        Mix_HaltChannel(channel);
    }

    Bus<Effect>::Bus(const double& volume, const int& max_channels)
        : volume(volume)
    {
        Mix_AllocateChannels(max_channels);
    }

    Bus<Effect>::~Bus() { stop(); }
}
