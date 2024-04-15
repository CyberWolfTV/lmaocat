#ifndef RAINBOW_HPP
#define RAINBOW_HPP

#include <chrono>
#include <cstddef>
#include <cstdint>
#include <cwchar>
#include <istream>
#include <string>

#include "ANSI/ANSI.hpp"
#include "Args/Args.hpp"


class Rainbow {
public:
    explicit Rainbow(const Args& settings);

    void cat(std::wistream& input);

private:
    void print_plain(const std::wstring& line, State& ansi_state) const;
    void print_animated(const std::wstring& line, State& ansi_state);

    void rainbow(std::size_t index, RGB& color) const;

    static auto getline(std::wistream& input, std::wstring& line, bool& chomped) -> std::wistream&;

    double state{};

    ANSI ansi_handler;

    bool truecolor_enabled{};

    const double frequency;
    const double spread;

    const std::int64_t duration;
    const bool animate;
    const std::chrono::milliseconds sleep_time;

    struct Signal_Handler {
        static void setup();

    private:
        static void handler(int signal);
        inline static bool handler_set = false;
    };
};

#endif
