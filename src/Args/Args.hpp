#ifndef ARGS_HPP
#define ARGS_HPP

#include <vector>
#include <filesystem>

namespace fs = std::filesystem;


class Args {
public:
    Args(int argc, char *argv[]);

    bool help       = false;
    bool version    = false;

    bool invert     = false;
    bool truecolor  = false;

    bool force      = false;

    double spread    = 3.0;
    double frequency = 0.1;
    long   seed      = 0;

    bool   animate   = false;
    long   duration  = 12;
    double speed     = 20.0;

    std::vector<fs::path> files{};

private:
    [[nodiscard]] static bool is_arg(const char* full, const char* shortcut, const char* arg);

    template<typename T>
    [[nodiscard]] static bool get_value(const char* full, const char* shortcut, T &value, int &i, char* argv[]);

    template<typename T>
    [[nodiscard]] static T get_number(const char* arg, const char* value);

    void validate() const;

    static void invalid_arg_exit_error(const char *arg, const char *value);
};

#endif
