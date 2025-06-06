#pragma once
#include "floor/audio/memory.h"
#include "floor/events/type.h"
#include "floor/structures.h"

namespace Floor::Audio
{
    struct Mixer;

    namespace Buses
    {
        using namespace Floor::Audio::Memory;

        template <AudioTypename AudioType>
        struct Bus;

        template <>
        struct Bus<Music> final
        {
        private:
            struct Volume final : private PercentValue<uint8_t>
            {
            private:
                double set_value(const uint8_t& value) override;

            public:
                [[nodiscard]] double get() const;
                void set(); // re-update
                void set(const double& percent);

                explicit Volume(const double& percent = 1.0);
            };

            explicit Bus(const double& volume = 1.0);

            friend struct Mixer;

        protected:
            Item<Music> current_music;

        public:
            Volume volume;

            bool play(const Item<Music>& music, bool pause = false, int loops = 0);
            const Item<Music>& get_current_music();
            Events::Timing::Time get_position();
            static void seek(const int64_t& new_pos);
            static bool has_song();
            static bool is_playing();
            static void pause();
            static void resume();
            void stop();

            Bus() = delete;
            Bus(const Bus&) = delete;
            Bus& operator=(const Bus&) = delete;
            ~Bus();
        };

        template <>
        struct Bus<Effect> final
        {
        private:
            struct Volume final : private PercentValue<uint8_t>
            {
            private:
                PercentValue channel_volume{0, MIX_MAX_VOLUME, 1.0, true, this}; // minh họa

                double set_value(const uint8_t& value) override
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

            public:
                [[nodiscard]] double get(const int& channel = -1) const;

                void set(); // re-update
                void set(const double& percent, const int& channel = -1);

                explicit Volume(const double& percent = 1.0);
            };

            explicit Bus(const double& volume = 1.0, const int& max_channels = 8);

            friend struct Mixer;

        public:
            Volume volume;
            int play(const Item<Effect>& sound, const double& volume = 1);
            static void stop(const int& channel = -1);

            Bus() = delete;
            Bus(const Bus&) = delete;
            Bus& operator=(const Bus&) = delete;
            ~Bus();
        };
    }
}
