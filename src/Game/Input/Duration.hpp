// src/Game/Input/Duration.hpp

#include <chrono>

#pragma once

class Duration {
public:
    using clock = std::chrono::steady_clock;
    using TimePoint = clock::time_point;

    explicit Duration(std::chrono::nanoseconds ns = std::chrono::nanoseconds(0))
        : m_duration(ns) {}

    static Duration fromSeconds(double seconds) {
        return Duration(std::chrono::duration_cast<std::chrono::nanoseconds>(std::chrono::duration<double>(seconds)));
    }

    static Duration fromMilliseconds(double milliseconds) {
        return Duration(std::chrono::duration_cast<std::chrono::nanoseconds>(std::chrono::duration<double, std::milli>(milliseconds)));
    }

    static Duration fromMicroseconds(double microseconds) {
        return Duration(std::chrono::duration_cast<std::chrono::nanoseconds>(std::chrono::duration<double, std::micro>(microseconds)));
    }

    double asSeconds() const {
        return std::chrono::duration<double>(m_duration).count();
    }

    double asMilliseconds() const {
        return std::chrono::duration<double, std::milli>(m_duration).count();
    }

    double asMicroseconds() const {
        return std::chrono::duration<double, std::micro>(m_duration).count();
    }

    int64_t asNanoseconds() const {
        return m_duration.count();
    }

    static TimePoint now() {
        return clock::now();
    }

    TimePoint fromNow() const {
        return clock::now() + m_duration;
    }

    static Duration since(const TimePoint& time_point) {
        return Duration(std::chrono::duration_cast<std::chrono::nanoseconds>(clock::now() - time_point));
    }

    Duration operator+(const Duration& other) const {
        return Duration(m_duration + other.m_duration);
    }

    Duration operator-(const Duration& other) const {
        return Duration(m_duration - other.m_duration);
    }

    TimePoint operator+(const TimePoint& time_point) const {
        return time_point + m_duration;
    }

    TimePoint operator-(const TimePoint& time_point) const {
        return time_point - m_duration;
    }

private:
    std::chrono::nanoseconds m_duration;
};

