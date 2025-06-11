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
        bool clamped = true;
        const PercentValue* parent = nullptr;

        [[nodiscard]] NumberType get_value(const double& custom_percent) const
        {
            const auto parent_percent = (parent) ? parent->get_percent() : 1.0;
            return Utilities::Math::to_value(custom_percent * parent_percent, min, max, false);
        }

        [[nodiscard]] NumberType get_value() const { return get_value(percent); }

        [[nodiscard]] const double& get_percent() const { return percent; }

        [[nodiscard]] double get_percent(const NumberType& custom_value) const
        {
            const auto parent_percent = parent ? parent->get_percent() : 1.0;
            if (Utilities::Math::is_equal(parent_percent, 0.0)) parent_percent = 1.0;

            return Utilities::Math::to_percent(custom_value.value(), min, max, false) / parent_percent;
        }

        [[nodiscard]] std::pair<NumberType, NumberType> get_range() const
        {
            return {min, max};
        }

        virtual double set_value(const NumberType& value)
        {
            return percent = get_percent(value);
        }

        NumberType set_percent(const double& percent)
        {
            const auto value = get_value(percent);
            set_value(value);
            return value;
        }

        NumberType set_range(const NumberType& min, const NumberType& max)
        {
            this->min = min;
            this->max = max;
            return set_percent(percent);
        }

        PercentValue(const NumberType& min, const NumberType& max, const double percent = 1.0,
                     const bool clamped = true, const PercentValue* parent = nullptr)
            : min(min), max(max), percent(percent), clamped(clamped), parent(parent)
        {
        }

        virtual ~PercentValue() = default;
    };
}
