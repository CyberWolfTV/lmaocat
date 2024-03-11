#include "ANSI.hpp"

#include <string>
#include <cstdio>
#include <cstddef>


ANSI::ANSI(const bool invert) {
    if (invert) {
        std::wmemcpy(this->color_template, L"\x1B[48;2;%d;%d;%dm%lc%ls", std::wcslen(this->color_template));
        std::wmemcpy(this->color_end, L"\x1B[49m", std::wcslen(this->color_end));
    }
}


/*
 * https://stackoverflow.com/a/14693789
 * REGEX:
 *   \x1B  # ESC
 *   (?:   # 7-bit C1 Fe (except CSI)
 *       [@-Z\\-_]
 *       |     # or [ for CSI, followed by a control sequence
 *       \[
 *       [0-?]*  # Parameter bytes
 *       [ -/]*  # Intermediate bytes
 *       [@-~]   # Final byte
 *   )
 */

void ANSI::read(const std::wstring &line, std::size_t &i, State &s) {
    std::putwchar(L'\x1B');
    ++i; // cuz first one is ESC
    s = State::ESC;

    read_incomplete(line, i, s);
}


void ANSI::read_incomplete(const std::wstring &line, std::size_t &i, State &s) {
    const std::size_t len = line.length();
    while (i != len) {
        int ascii_code = static_cast<unsigned char>(line[i]);
        switch (s) {
            case State::ESC:
                if (is_CSI(ascii_code)) {
                    s = State::PARAM;
                    std::putwchar(L'\x5B');
                    ++i;
                }
                else if (is_in_c_zero(ascii_code)) {
                    s = State::NONE;
                    std::putwchar(line[i]);
                    ++i;
                }
                else {
                    s = State::NONE;
                    return;
                }
                break;

            case State::PARAM:
                while (is_in_param(ascii_code)) {
                    std::putwchar(line[i]);
                    ++i;
                    if (i == len) {
                        return;
                    }
                    ascii_code = static_cast<unsigned char>(line[i]);
                }
                s = State::INTER;
                break;

            case State::INTER:
                while (is_in_inter(ascii_code)) {
                    std::putwchar(line[i]);
                    ++i;
                    if (i == len) {
                        return;
                    }
                    ascii_code = static_cast<unsigned char>(line[i]);
                }
                s = State::FINAL;
                break;

            case State::FINAL:
                if (is_in_final(ascii_code)) {
                    std::putwchar(line[i]);
                    ++i;
                }
                s = State::NONE;
                return;

            case State::NONE:
                return;
        }
    }
}
