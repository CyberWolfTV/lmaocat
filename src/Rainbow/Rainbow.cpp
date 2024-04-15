#include "Rainbow.hpp"

#include <chrono>
#include <climits>
#include <csignal>
#include <cstddef>
#include <cstdint>
#include <cstdio>
#include <cwchar>
#include <istream>
#include <random>
#include <string>
#include <thread>

#include <unistd.h>

#include "ANSI/ANSI.hpp"
#include "Args/Args.hpp"


#define BASE_SPEED 1000.0
#define TAB 8


Rainbow::Rainbow(const Args &settings):
    frequency(settings.frequency),
    spread(settings.spread),
    duration(settings.duration),
    animate(settings.animate),
    sleep_time(static_cast<int>((BASE_SPEED / settings.speed))) {

    bool truecolor = settings.truecolor;
    if (!settings.truecolor) {
        std::string mode{};
        if (std::getenv("COLORTERM") != nullptr) {
            mode = std::getenv("COLORTERM");
        }

        truecolor = (mode == "truecolor" || mode == "24bit");
    }

    // https://stackoverflow.com/a/59511764
    // 1 = std::cout -> STDOUT_FILENO
    this->truecolor_enabled = (settings.force || (isatty(STDOUT_FILENO) == 1 && truecolor));

    if (settings.seed == 0) {
        std::random_device r_device;
        std::mt19937 engine(r_device());
        std::uniform_int_distribution dist(INT_MIN, INT_MAX);
        this->state = static_cast<double>(dist(engine));
    } else {
        this->state = static_cast<double>(settings.seed);
    }

    this->ansi_handler = ANSI{ settings.invert };

    if (this->truecolor_enabled) {
        Signal_Handler::setup();
    }
}


void Rainbow::cat(std::wistream &input) {
    if (this->truecolor_enabled) {
        if (this->animate) {
            ANSI::hide_cursor();
        }

        bool chomped{};
        auto ansi_state = State::NONE;
        for (std::wstring line; getline(input, line, chomped);) {
            if (this->animate) {
                print_animated(line, ansi_state);
            } else {
                print_plain(line, ansi_state);
            }

            if (chomped) {
                std::putwchar(L'\n');
            }

            this->state += this->spread;
        }

        ANSI::reset_ansi_in_terminal();
    }
    else {
        bool chomped{};
        for (std::wstring line; getline(input, line, chomped);) {
            std::fputws(line.c_str(), stdout);

            if (chomped) {
                std::putwchar(L'\n');
            }
        }
    }
}


void Rainbow::print_plain(const std::wstring &line, State &ansi_state) const {
    std::size_t index = 0;

    if (ANSI::incomplete_ansi(ansi_state)) {
        ANSI::read_incomplete(line, index, ansi_state);
    }

    std::size_t counter = 0;
    RGB color;
    for (; index < line.length(); ++index) {
        if (ANSI::is_esc(line[index])) {
            ANSI::read(line, index, ansi_state);
            --index;
        }
        else if (line[index] == '\t') {
            for (int j = 0; j < TAB; ++j) {
                rainbow(counter, color);
                this->ansi_handler.paint(color, L' ');
                ++counter;
            }
        }
        else {
            rainbow(counter, color);
            this->ansi_handler.paint(color, line[index]);
            ++counter;
        }
    }
}


void Rainbow::print_animated(const std::wstring &line, State &ansi_state) {
    ANSI::save_cursor_position();

    const double state_backup          = this->state;
    const State state_before_line_read = ansi_state;

    for (std::int64_t i = 0; i < this->duration; ++i) {
        ANSI::restore_cursor_position();
        ansi_state = state_before_line_read;
        this->state += this->spread;
        this->print_plain(line, ansi_state);
        std::this_thread::sleep_for(this->sleep_time);
    }

    this->state = state_backup;
}


void Rainbow::rainbow(const std::size_t index, RGB &color) const {
    auto index_f = static_cast<double>(index);

    index_f += this->state;
    index_f /= this->spread;
    index_f *= this->frequency;

    constexpr double PI = 3.14159;

    constexpr double RED   = 0.0 * PI / 3.0;
    constexpr double GREEN = 2.0 * PI / 3.0;
    constexpr double BLUE  = 4.0 * PI / 3.0;

    const double red_f   = std::sin(index_f + RED)   * 127.0 + 128.0;
    const double green_f = std::sin(index_f + GREEN) * 127.0 + 128.0;
    const double blue_f  = std::sin(index_f + BLUE)  * 127.0 + 128.0;

    color.red   = static_cast<int>(red_f);
    color.green = static_cast<int>(green_f);
    color.blue  = static_cast<int>(blue_f);
}


auto Rainbow::getline(std::wistream &input, std::wstring &line, bool &chomped) -> std::wistream& {
    line.clear();
    chomped = false;

    for (wchar_t character{}; input.get(character);) {
        if (character != L'\n') {
            line += character;
        }
        else {
            chomped = true;
            break;
        }
    }

    if (input.eof() && !line.empty()) {
        input.clear();
    }

    return input;
}


void Rainbow::Signal_Handler::setup() {
    if (!handler_set) {
        std::signal(SIGINT,  handler);
        std::signal(SIGKILL, handler);
        std::signal(SIGTERM, handler);
        std::signal(SIGQUIT, handler);
        std::signal(SIGABRT, handler);

        handler_set = true;
    }
}


void Rainbow::Signal_Handler::handler(const int signal) {
    ANSI::reset_ansi_in_terminal();

    const int exit_code = 128 + signal;
    std::exit(exit_code);
}
