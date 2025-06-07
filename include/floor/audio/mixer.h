#pragma once
#include "floor/audio/buses.h"

namespace Floor::Audio
{
    struct Mixer final
    {
        static std::unique_ptr<Bus<Music>> music;
        static std::unique_ptr<Bus<Effect>> effects;

    private:
        struct Volume final : private PercentValue<uint8_t>
        {
            // chỉ friend để lấy constructor của base :v
            friend struct Bus<Music>;
            friend struct Bus<Effect>;

        private:
            double set_value(const uint8_t& value) override;

        public:
            [[nodiscard]] double get() const;
            void set(const double& percent);

            explicit Volume(const double& volume = 1.0);
        };

    protected:
        static inline bool initialized = false;

    public:
        static std::unique_ptr<Volume> volume;

        static const bool& is_initialized();
        static void init(const MIX_InitFlags& flags, const SDL_AudioSpec& spec,
                         const std::tuple<double, double, double>& volumes = {1, 1, 1},
                         const int& max_channels = 8, const SDL_AudioDeviceID& device = 0);
        static void stop();
        static void quit();

        Mixer() = delete;
        Mixer(const Mixer&) = delete;
        Mixer& operator=(const Mixer&) = delete;
        ~Mixer();
    };
}
