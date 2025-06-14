﻿#pragma once
#include <algorithm>
#include <filesystem>
#include <chrono>
#include <map>
#include <queue>
#include <SDL3/SDL_pixels.h>
#include <SDL3/SDL_rect.h>

#include "floor/render/config.h"

namespace fs = std::filesystem;

namespace Floor::Utilities
{
    namespace Math
    {
        bool is_equal(const double& variable, const double& value, const double& epsilon = 0.001);
        bool is_integer(const double& value, const double& epsilon = 0.001f);
        float centre(const float& size, const float& window_size);
        SDL_FPoint centre(const SDL_FPoint& size, const SDL_FPoint& window_size);
        double degree_to_radian(const double& degree);
        double radian_to_degree(const double& radian);
        double round(const double& value, const uint8_t& precision_num = 0);
        std::pair<double, double> decimal_separate(const double& value);
        size_t count_digits(const size_t& value);

        template <typename IntegerType>
        IntegerType circle_mod(const IntegerType& value, const IntegerType& mod)
        {
            if (mod == 0) return value;
            return (value % mod + mod) % mod;
        }

        namespace FPoint
        {
            SDL_FPoint operator+(const SDL_FPoint& a, const SDL_FPoint& b);
            SDL_FPoint& operator+=(SDL_FPoint& a, const SDL_FPoint& b);

            SDL_FPoint operator-(const SDL_FPoint& a);
            SDL_FPoint operator-(const SDL_FPoint& a, const SDL_FPoint& b);
            SDL_FPoint& operator-=(SDL_FPoint& a, const SDL_FPoint& b);

            SDL_FPoint operator*(const SDL_FPoint& a, const SDL_FPoint& b);
            SDL_FPoint operator*(const SDL_FPoint& point, const float& value);
            SDL_FPoint& operator*=(SDL_FPoint& a, const SDL_FPoint& b);
            SDL_FPoint& operator*=(SDL_FPoint& point, const float& value);

            SDL_FPoint operator/(const SDL_FPoint& a, const SDL_FPoint& b);
            SDL_FPoint operator/(const SDL_FPoint& point, const float& value);
            SDL_FPoint& operator/=(SDL_FPoint& a, const SDL_FPoint& b);
            SDL_FPoint& operator/=(SDL_FPoint& point, const float& value);

            bool operator==(const SDL_FPoint& a, const SDL_FPoint& b);
            bool operator!=(const SDL_FPoint& a, const SDL_FPoint& b);

            SDL_Point to_integer_point(const SDL_FPoint& point);
            SDL_FPoint to_float_point(const SDL_Point& point);
            SDL_FPoint to_float_point(const double& x, const double& y);
        }

        template <typename NumberType>
        double to_percent(const NumberType& value, const NumberType& from, const NumberType& to,
                          const bool clamp = true)
        {
            if (from == to) return 1.0f;
            const auto res = static_cast<double>(value - from) / static_cast<double>(to - from);
            return (!clamp) ? res : std::clamp(res, 0.0, 1.0);
        }

        template <typename NumberType>
        NumberType to_value(double percent, const NumberType& from, const NumberType& to, const bool clamp = true)
        {
            if (from == to) return to;
            if (clamp) percent = std::clamp(percent, 0.0, 1.0);
            return static_cast<NumberType>(from + (to - from) * percent);
        }

        template <typename ComparableType>
        bool in_range(const ComparableType& min, const ComparableType& max,
                      const ComparableType& value, const bool include_equal_left = true,
                      const bool include_equal_right = true)
        {
            if (include_equal_left && include_equal_right) return (min <= value && value <= max);
            if (include_equal_left) return (min <= value && value < max);
            if (include_equal_right) return (min < value && value <= max);
            return (min < value && value < max);
        }

        template <typename ComparableType>
        bool in_offset_range(const ComparableType& correct_value, const ComparableType& offset,
                             const ComparableType& value, const bool include_equal_left = true,
                             const bool include_equal_right = true)
        {
            return in_range(correct_value - offset, correct_value + offset, value, include_equal_left,
                            include_equal_right);
        }

        template <typename ComparableType>
        bool has_contain_part(const std::pair<ComparableType, ComparableType>& a,
                              const std::pair<ComparableType, ComparableType>& b)
        {
            return in_range(a.first, a.second, b.first) || in_range(a.first, a.second, b.second)
                || in_range(b.first, b.second, a.first) || in_range(b.first, b.second, a.second);
        }
    }

    namespace Time
    {
        std::string get_current_time(const char* format = "%Y-%m-%d_%H-%M-%S");

        template <typename DurationType>
        long long get_duration(
            const std::chrono::time_point<std::chrono::system_clock>& previous,
            const std::chrono::time_point<std::chrono::system_clock>& current = std::chrono::system_clock::now())
        {
            return std::chrono::duration_cast<DurationType>(current - previous).count();
        }
    }

    namespace String
    {
        std::string trim(std::string str, bool only_trim_right = false);
        std::vector<std::string> split(const std::string& input, char delimiter = ',', bool only_two_part = false);
    }

    namespace Path
    {
        fs::path normalize_path(fs::path path);
        fs::path make_valid_long_path(fs::path path);
    }

    namespace Render
    {
        SDL_FRect merge_pos_size(const SDL_FPoint& pos, const SDL_FPoint& size);
        SDL_FPoint get_pos_from_rect(const SDL_FRect& rect);
        SDL_FPoint get_size_from_rect(const SDL_FRect& rect);
        bool is_point_in_rect(const SDL_FPoint& point, const SDL_FRect& rect);
        SDL_Color to_sdl_color(const Floor::Render::Color& color, const uint8_t& alpha = Floor::Render::SDL_MAX_ALPHA);
    }

    namespace Container
    {
        template <typename Container>
        typename Container::iterator get_last_element_iterator(Container& container)
        {
            if (container.empty()) return container.end();
            return std::prev(container.end());
        }

        template <typename Container>
        typename Container::const_iterator get_last_element_iterator(const Container& container)
        {
            if (container.empty()) return container.cend();
            return std::prev(container.cend());
        }

        template <typename LeftType, typename RightType, typename Comparator>
        bool get_front_dual_queue( // đúng thì lấy bên phải (vì bên phải luôn right :>)
            const std::queue<LeftType>& left, const std::queue<RightType>& right, Comparator comp)
        {
            if (left.empty() && right.empty()) return false;
            if (right.empty()) return false;
            if (left.empty()) return true;
            return comp(left.front(), right.front());
        }

        template <typename Type>
        Type get_front_and_pop(std::queue<Type>& queue)
        {
            Type front = queue.front();
            queue.pop();
            return front;
        }

        template <typename Type>
        std::priority_queue<Type> get_front_and_pop(std::priority_queue<Type>& queue)
        {
            Type front = queue.top();
            queue.pop();
            return front;
        }

        template <typename KeyType, typename ValueType>
        std::queue<typename std::multimap<KeyType, ValueType>::value_type> multimap_to_queue(
            const std::multimap<KeyType, ValueType>& map)
        {
            std::queue<typename std::multimap<KeyType, ValueType>::value_type> res;
            for (const auto& item : map)
                res.push(item);
            return res;
        }
    }

    namespace Pointer
    {
        template <typename Type>
        [[nodiscard]] std::shared_ptr<Type> check_weak(const std::weak_ptr<Type>& ptr)
        {
            if (ptr.expired()) return nullptr;
            return ptr.lock();
        }
    }
}
