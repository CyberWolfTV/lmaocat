#ifndef ANSI_HANDLER_HPP
#define ANSI_HANDLER_HPP

#include <string>
#include <cstdio>
#include <cstddef>


struct RGB {
    inline RGB(int red, int green, int blue);
    const int red, green, blue;
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
    ANSI()= default;

    inline void paint(const RGB &color, wchar_t c) const;

    static void read(const std::wstring &line, std::size_t &i, State &s);

    static void read_incomplete(const std::wstring &line, std::size_t &i, State &s);

    [[nodiscard]] static inline bool is_esc(wchar_t c);

    [[nodiscard]] static inline bool incomplete_ansi(State s);

    static inline void hide_cursor();

    static inline void save_cursor_position();

    static inline void restore_cursor_position();

    static inline void reset_ansi_in_terminal();

private:
    wchar_t color_template[23] = L"\x1B[38;2;%d;%d;%dm%lc%ls";
    wchar_t color_end[6] = L"\x1B[39m";

    // [ -/] = Intermediate bytes
    [[nodiscard]] static inline bool is_in_inter(int ascii_code);

    // [0-?] = Parameter bytes
    [[nodiscard]] static inline bool is_in_param(int ascii_code);

    // [@-~] = Final byte
    [[nodiscard]] static inline bool is_in_final(int ascii_code);

    // [@-_] = c0 with CSI
    [[nodiscard]] static inline bool is_in_c_zero(int ascii_code);

    // \[ = CSI
    [[nodiscard]] static inline bool is_CSI(int ascii_code);
};



inline RGB::RGB(const int red, const int green, const int blue): red(red), green(green), blue(blue) {}


inline void ANSI::paint(const RGB &color, const wchar_t c) const {
    std::wprintf(color_template, color.red, color.green, color.blue, c, color_end);
}


inline bool ANSI::is_esc(const wchar_t c) {
    return (c == L'\x1B');
}


inline bool ANSI::incomplete_ansi(const State s) {
    return (s != State::NONE);
}


inline void ANSI::hide_cursor() {
    std::wprintf(L"\x1B[?25l");
}


inline void ANSI::save_cursor_position() {
    constexpr wchar_t control_sequence[] = L"\x1B" L"7";
    std::wprintf(control_sequence);
}


inline void ANSI::restore_cursor_position() {
    constexpr wchar_t control_sequence[] = L"\x1B" L"8";
    std::wprintf(control_sequence);
}


inline void ANSI::reset_ansi_in_terminal() {
    std::wprintf(L"\x1B[m\x1B[?25h\x1B[?1;5;2004l");
}


inline bool ANSI::is_in_inter(const int ascii_code) {
    return (ascii_code >= 32) && (ascii_code <= 47);
}


inline bool ANSI::is_in_param(const int ascii_code) {
    return (ascii_code >= 48) && (ascii_code <= 63);
}


inline bool ANSI::is_in_final(const int ascii_code) {
    return (ascii_code >= 64) && (ascii_code <= 126);
}


inline bool ANSI::is_in_c_zero(const int ascii_code) {
    return (ascii_code >= 64) && (ascii_code <= 95);
}


inline bool ANSI::is_CSI(const int ascii_code) {
    return (ascii_code == 91);
}

#endif
