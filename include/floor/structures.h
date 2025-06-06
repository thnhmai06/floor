#pragma once
#include "floor/utilities.h"

namespace Floor
{
    template <typename NumberType>
    struct PercentValue
    {
    private:
        NumberType min, max;
        double percent = 1;

    public:
        const PercentValue* parent = nullptr;
        bool clamped = true;

        [[nodiscard]] NumberType get_value(std::optional<double> custom_percent = std::nullopt) const
        {
            const auto parent_percent = (parent) ? parent->get_percent(true) : 1.0;
            return Utilities::Math::to_value(custom_percent.value_or(this->percent) * parent_percent, min, max, false);
        }

        [[nodiscard]] double get_percent(std::optional<NumberType> custom_value = std::nullopt) const
        {
            const auto parent_percent = parent ? parent->get_percent(true) : 1.0;
            if (Utilities::Math::is_equal(parent_percent, 0.0)) parent_percent = 1.0;

            if (!custom_value.has_value()) return percent; // vì percent đã là giá trị not final rồi
            return Utilities::Math::to_percent(custom_value.value(), min, max, false) / parent_percent;
        }
        [[nodiscard]] std::pair<NumberType, NumberType> get_range() const
        {
            return { min, max };
        }

        virtual double set_value(const NumberType& value)
        {
            return percent = get_percent(true, value);
        }
        NumberType set_percent(double percent)
        {
            const auto value = get_value(true, percent);
            set_value(value);
            return value;
        }
        double set_range(const NumberType& min, const NumberType& max, const bool fixed_value = false)
        {
            if (fixed_value)
                percent = set_value(get_value(false), false);
            this->min = min;
            this->max = max;
            return percent;
        }

        PercentValue(const NumberType& min, const NumberType& max, const double percent = 1.0,
                     const bool clamped = true, const PercentValue* parent = nullptr)
            : min(min), max(max), percent(percent), parent(parent), clamped(clamped)
        {
        }

        virtual ~PercentValue() = default;
    };
}
