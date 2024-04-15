#ifndef ANSI_HANDLER_HPP
#define ANSI_HANDLER_HPP

#include <cwchar>
#include <string>
#include <cstdio>
#include <cstddef>


struct RGB {
    RGB() = default;
    int red{}, green{}, blue{};
};


enum class State {
    NONE,
    ESC,
    PARAM,
    INTER,
    FINAL
};


class ANSI {
public:
    explicit ANSI(bool invert);
    ANSI() = default;

    inline void paint(const RGB &color, wchar_t character) const;

    static void read(const std::wstring &line, std::size_t &index, State &state);

    static void read_incomplete(const std::wstring &line, std::size_t &index, State &state);

    [[nodiscard]] static inline auto is_esc(wchar_t character) -> bool;

    [[nodiscard]] static inline auto incomplete_ansi(State state) -> bool;

    static inline void hide_cursor();

    static inline void save_cursor_position();

    static inline void restore_cursor_position();

    static inline void reset_ansi_in_terminal();

private:
    wchar_t color_template[23] = L"\x1B[38;2;%d;%d;%dm%lc%ls";
    wchar_t color_end[6] = L"\x1B[39m";

    inline static constexpr wchar_t ESC = L'\x1B';

    static constexpr int SPACE        =  32;
    static constexpr int SLASH        =  47;
    static constexpr int ZERO         =  48;
    static constexpr int QUESTIONMARK =  63;
    static constexpr int ADDRESS      =  64;
    static constexpr int CSI          =  91;
    static constexpr int UNDERSCORE   =  95;
    static constexpr int TILDE        = 126;

    // [ -/] = Intermediate bytes
    [[nodiscard]] static inline auto is_in_inter(int ascii_code) -> bool;

    // [0-?] = Parameter bytes
    [[nodiscard]] static inline auto is_in_param(int ascii_code) -> bool;

    // [@-~] = Final byte
    [[nodiscard]] static inline auto is_in_final(int ascii_code) -> bool;

    // [@-_] = c0 with CSI
    [[nodiscard]] static inline auto is_in_c_zero(int ascii_code) -> bool;

    // [ = CSI
    [[nodiscard]] static inline auto is_CSI(int ascii_code) -> bool;
};


inline void ANSI::paint(const RGB &color, const wchar_t character) const {
    std::wprintf(color_template, color.red, color.green, color.blue, character, color_end);
    std::putwchar(ESC);
}


inline auto ANSI::is_esc(const wchar_t character) -> bool {
    return (character == ESC);
}


inline auto ANSI::incomplete_ansi(const State state) -> bool {
    return (state != State::NONE);
}


inline void ANSI::hide_cursor() {
    std::fputws(L"\x1B[?25l", stdout);
}


inline void ANSI::save_cursor_position() {
    std::putwchar(ESC);
    std::putwchar(L'7');
}


inline void ANSI::restore_cursor_position() {
    std::putwchar(ESC);
    std::putwchar(L'8');
}


inline void ANSI::reset_ansi_in_terminal() {
    std::fputws(L"\x1B[m\x1B[?25h\x1B[?1;5;2004l", stdout);
}


inline auto ANSI::is_in_inter(const int ascii_code) -> bool {
    return (ascii_code >= SPACE) && (ascii_code <= SLASH);
}


inline auto ANSI::is_in_param(const int ascii_code) -> bool {
    return (ascii_code >= ZERO) && (ascii_code <= QUESTIONMARK);
}


inline auto ANSI::is_in_final(const int ascii_code) -> bool {
    return (ascii_code >= ADDRESS) && (ascii_code <= TILDE);
}


inline auto ANSI::is_in_c_zero(const int ascii_code) -> bool {
    return (ascii_code >= ADDRESS) && (ascii_code <= UNDERSCORE);
}


inline auto ANSI::is_CSI(const int ascii_code) -> bool {
    return (ascii_code == CSI);
}

#endif
