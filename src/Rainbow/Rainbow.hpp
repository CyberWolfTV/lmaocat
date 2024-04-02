#ifndef RAINBOW_HPP
#define RAINBOW_HPP


#include <chrono>
#include <string>
#include <cstddef>
#include <sstream>

#include "Args/Args.hpp"
#include "ANSI/ANSI.hpp"


class Rainbow {
public:
    explicit Rainbow(const Args &settings);

    void cat(std::wistream &is);

private:
    void print_plain(const std::wstring &line, State &ansi_state) const;
    void print_animated(const std::wstring &line, State &ansi_state);

    [[nodiscard]] RGB rainbow(std::size_t index) const;

    double state{};

    ANSI ansi_handler{};

    bool truecolor_enabled{};

    const double frequency;
    const double spread;

    const long duration;
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
