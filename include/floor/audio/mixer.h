#pragma once
#include <SDL3/SDL_audio.h>
#include "floor/audio/buses.h"

namespace Floor::Audio
{
    struct Mixer final
    {
        static Buses::Bus<Music> music;
        static Buses::Bus<Effect> effects;

    private:
        struct Volume final : private PercentValue<uint8_t>
        {
        private:
            double set_value(const uint8_t& value) override
            {
                const auto percent = PercentValue::set_value(value);
                music.volume.set();
                effects.volume.set();

                return percent;
            }

            friend Buses::Bus<Music>;
            friend Buses::Bus<Effect>;

        public:
            [[nodiscard]] double get() const { return get_percent(false); }
            void set(const double& percent) { set_percent(percent); }

            explicit Volume(const double& percent = 1.0, const PercentValue* parent = nullptr)
                : PercentValue(0, MIX_MAX_VOLUME, percent, true, parent)
            {
            }
        };

    public:
        static Volume volume;

        static void stop_all();
        static void init(const MIX_InitFlags& flags, const SDL_AudioSpec& spec,
                  const double& master_volume, const double& music_volume,
                  const double& effect_volume, const int32_t& max_channels = 8);
        static void quit();

        ~Mixer();
        Mixer() = delete;
        Mixer(const Mixer&) = delete;
        Mixer& operator=(const Mixer&) = delete;
    };
}
