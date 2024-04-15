#ifndef ARGS_HPP
#define ARGS_HPP

#include <cstdint>
#include <vector>
#include <filesystem>

namespace fs = std::filesystem;


#define DEFAULT_HELP      false
#define DEFAULT_VERSION   false

#define DEFAULT_INVERT    false
#define DEFAULT_TRUECOLOR false

#define DEFAULT_FORCE     false

#define DEFAULT_SPREAD    3.0
#define DEFAULT_FREQUENCY 0.1
#define DEFAULT_SEED      0

#define DEFAULT_ANIMATE   false
#define DEFAULT_DURATION  12
#define DEFAULT_SPEED     20.0


class Args {
public:
    Args(int argc, char *argv[]);

    bool    help      = DEFAULT_HELP;
    bool    version   = DEFAULT_VERSION;

    bool    invert    = DEFAULT_INVERT;
    bool    truecolor = DEFAULT_TRUECOLOR;

    bool    force     = DEFAULT_FORCE;

    double  spread    = DEFAULT_SPREAD;
    double  frequency = DEFAULT_FREQUENCY;
    int64_t seed      = DEFAULT_SEED;

    bool    animate   = DEFAULT_ANIMATE;
    int64_t duration  = DEFAULT_DURATION;
    double  speed     = DEFAULT_SPEED;

    std::vector<fs::path> files;

private:
    [[nodiscard]] static auto is_arg(const char* full, const char* shortcut, const char* arg) -> bool;

    template<typename T>
    [[nodiscard]] static auto get_value(const char* full, const char* shortcut, T &value, int &i, char* argv[]) -> bool;

    template<typename T>
    [[nodiscard]] static auto get_number(const char* arg, const char* value) -> T;

    void validate() const;

    static void invalid_arg_exit_error(const char *arg, const char *value);
};

#endif
