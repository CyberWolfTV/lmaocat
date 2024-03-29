#include "Rainbow.hpp"

#include <random>
#include <cstdio>
#include <chrono>
#include <string>
#include <thread>
#include <csignal>
#include <climits>
#include <cstddef>
#include <sstream>

#include <unistd.h>

#include "Args/Args.hpp"
#include "ANSI/ANSI.hpp"


#define TAB 8


Rainbow::Rainbow(const Args &settings):
            frequency(settings.frequency),
            spread   (settings.spread),
            duration (settings.duration),
            animate  (settings.animate),
            sleep_time(static_cast<int>((1000.0 / settings.speed))) {

    bool truecolor = settings.truecolor;
    if (!settings.truecolor) {
        std::string mode{};
        if (std::getenv("COLORTERM") != nullptr) {
            mode = std::getenv("COLORTERM");
        }

        if (mode == "truecolor" || mode == "24bit") {
            truecolor = true;
        } else {
            truecolor = false;
        }
    }

    // https://stackoverflow.com/a/59511764
    // 1 = std::cout -> STDOUT_FILENO
    this->truecolor_enabled = (settings.force || (isatty(STDOUT_FILENO) && truecolor));

    if (settings.seed == 0) {
        std::random_device r_device;
        std::mt19937 engine(r_device());
        std::uniform_int_distribution dist(INT_MIN, INT_MAX);
        this->state = static_cast<double>(dist(engine));
    } else {
        this->state = static_cast<double>(settings.seed);
    }

    this->ansi_handler = ANSI{settings.invert};

    if (this->truecolor_enabled) {
        signal_handler.setup();
    }
}


void Rainbow::cat(std::wistream &is) {
    if (this->truecolor_enabled) {
        if (this->animate) {
            ANSI::hide_cursor();
        }

        auto ansi_state = State::NONE;
        for (std::wstring line; std::getline(is, line);) {
            if (this->animate) {
                print_animated(line, ansi_state);
            }
            else {
                print_plain(line, ansi_state);
            }

            this->state += this->spread;
            std::putwchar(L'\n');
        }

        ANSI::reset_ansi_in_terminal();
    }
    else {
        for (std::wstring line; std::getline(is, line);) {
            std::wprintf(L"%ls\n", line.c_str());
        }
    }
}


void Rainbow::print_plain(const std::wstring &line, State &ansi_state) const {
    std::size_t i = 0;

    if (ANSI::incomplete_ansi(ansi_state)) {
        ANSI::read_incomplete(line, i, ansi_state);
    }

    std::size_t counter = 0;
    for (; i < line.length(); ++i) {
        if (ANSI::is_esc(line[i])) {
            ANSI::read(line, i, ansi_state);
            --i;
        }
        else if (line[i] == '\t') {
            for (int j = 0; j < TAB; ++j) {
                this->ansi_handler.paint(rainbow(counter), L' ');
                ++counter;
            }
        }
        else {
            this->ansi_handler.paint(rainbow(counter), line[i]);
            ++counter;
        }
    }
}


void Rainbow::print_animated(const std::wstring &line, State &ansi_state) {
    ANSI::save_cursor_position();

    const double state_backup = this->state;
    const State state_before_line_read = ansi_state;

    for (long i = 0; i < this->duration; ++i) {
        ANSI::restore_cursor_position();
        ansi_state = state_before_line_read;
        this->state += this->spread;
        this->print_plain(line, ansi_state);
        std::this_thread::sleep_for(this->sleep_time);
    }

    this->state = state_backup;
}


RGB Rainbow::rainbow(const std::size_t index) const {
    auto i = static_cast<double>(index);

    i += this->state;
    i /= this->spread;
    i *= this->frequency;

    constexpr double PI = 3.14159;

    constexpr double RED   = 0.0 * PI / 3.0;
    constexpr double GREEN = 2.0 * PI / 3.0;
    constexpr double BLUE  = 4.0 * PI / 3.0;

    const double red_f   = std::sin(i + RED)   * 127.0 + 128.0;
    const double green_f = std::sin(i + GREEN) * 127.0 + 128.0;
    const double blue_f  = std::sin(i + BLUE)  * 127.0 + 128.0;

    const auto red   = static_cast<int>(red_f);
    const auto green = static_cast<int>(green_f);
    const auto blue  = static_cast<int>(blue_f);

    return RGB{red, green, blue};
}


void Rainbow::Signal_Handler::setup() {
    if (!this->handler_set) {
        std::signal(SIGINT,  this->handler);
        std::signal(SIGKILL, this->handler);
        std::signal(SIGTERM, this->handler);

        this->handler_set = true;
    }
}


void Rainbow::Signal_Handler::handler(const int signal) {
    ANSI::reset_ansi_in_terminal();
    std::exit(128 + signal);
}
