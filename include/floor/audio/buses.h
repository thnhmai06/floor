#pragma once
#include "floor/audio/memory.h"
#include "floor/structures.h"
#include "floor/events/type.h"

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
                void set();
                void set(const double& percent);

                explicit Volume(const double& percent = 1.0);
            };

            MusicMemory::Item current;

            explicit Bus(const double& volume = 1.0);

            friend Mixer;

        public:
            Volume volume;

            void play(const MusicMemory::Item& music, bool pause = false, int loops = 0);
            const MusicMemory::Item& get_current();
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
            struct Volume final : PercentValue<uint8_t>
            {
            private:
                double set_value(const uint8_t& value) override
                {
                    ...
                    Mix_Volume(-1, value);
                    return PercentValue::set_value(value);
                }

            public:
                [[nodiscard]] double get() const
                {
                    return get_percent(false);
                }

                void set(const double& percent)
                {
                    set_percent(percent);
                }

                explicit Volume(const double& percent = 1.0, const PercentValue* parent = nullptr)
                    : PercentValue(0, MIX_MAX_VOLUME, percent, true, parent)
                {
                }
            };

        public:
            Volume volume;

            [[nodiscard]] double get_channel_volume(const int& channel) const;
            double set_volume(const double& percent);
            double set_volume(const EffectMemory::Item& sound, double percent) const;
            double set_volume(const int& channel, double percent) const;
            int play(const EffectMemory::Item& sound, const std::optional<double>& volume = std::nullopt) const;
            static void stop(const int& channel = -1);

            explicit Bus(const double& volume = 1.0, const int& max_channels = 8);
        };
    }
}
