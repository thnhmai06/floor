#pragma once
#include "floor/audio/memory.h"
#include "floor/events/type.h"
#include "floor/structures.h"

namespace Floor::Audio
{
    struct Mixer;

    template <AudioTypename Audio>
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
        Music::weak_type current_music; // lazy

    public:
        Volume volume;

        bool play(const Music& music, bool pause = false, int loops = 0);
        const Music::weak_type& get_current_music();
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

            double set_value(const uint8_t& value) override;

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
        int play(const Effect& sound,
            const double& volume = 1, const int& loops = 0, const int& channel = -1);
        static void stop(const int& channel = -1);

        Bus() = delete;
        Bus(const Bus&) = delete;
        Bus& operator=(const Bus&) = delete;
        ~Bus();
    };
}
