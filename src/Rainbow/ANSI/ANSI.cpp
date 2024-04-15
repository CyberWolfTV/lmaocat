#include "ANSI.hpp"

#include <cwchar>
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

void ANSI::read(const std::wstring &line, std::size_t &index, State &state) {
    std::putwchar(ESC);
    ++index; // cuz first one is ESC
    state = State::ESC;

    read_incomplete(line, index, state);
}


void ANSI::read_incomplete(const std::wstring &line, std::size_t &index, State &state) {
    const std::size_t len = line.length();
    while (index != len) {
        int ascii_code = static_cast<unsigned char>(line[index]);
        switch (state) {
            case State::ESC:
                if (is_CSI(ascii_code)) {
                    state = State::PARAM;
                    std::putwchar(CSI);
                    ++index;
                }
                else if (is_in_c_zero(ascii_code)) {
                    state = State::NONE;
                    std::putwchar(line[index]);
                    ++index;
                }
                else {
                    state = State::NONE;
                    return;
                }
                break;

            case State::PARAM:
                while (is_in_param(ascii_code)) {
                    std::putwchar(line[index]);
                    ++index;
                    if (index == len) {
                        return;
                    }
                    ascii_code = static_cast<unsigned char>(line[index]);
                }
                state = State::INTER;
                break;

            case State::INTER:
                while (is_in_inter(ascii_code)) {
                    std::putwchar(line[index]);
                    ++index;
                    if (index == len) {
                        return;
                    }
                    ascii_code = static_cast<unsigned char>(line[index]);
                }
                state = State::FINAL;
                break;

            case State::FINAL:
                if (is_in_final(ascii_code)) {
                    std::putwchar(line[index]);
                    ++index;
                }
                state = State::NONE;
                return;

            case State::NONE:
                return;
        }
    }
}
