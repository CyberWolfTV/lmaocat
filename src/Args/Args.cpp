#include "Args.hpp"

#include <vector>
#include <cerrno>
#include <cstdlib>
#include <cstring>
#include <type_traits>


Args::Args(const int argc, char *argv[]) {
    for (int i = 1; i < argc; ++i) {
        char *arg = argv[i];

        if (is_arg("--help", "-h", arg)) {
            this->help = true;
            continue;
        }
        if (is_arg("--version", "-v", arg)) {
            this->version = true;
            continue;
        }
        if (is_arg("--invert", "-i", arg)) {
            this->invert = true;
            continue;
        }
        if (is_arg("--truecolor", "-t", arg)) {
            this->truecolor = true;
            continue;
        }
        if (is_arg("--force", "-f", arg)) {
            this->force = true;
            continue;
        }
        if (is_arg("--animate", "-a", arg)) {
            this->animate = true;
            continue;
        }
        if (get_value<double>("--spread", "-p", this->spread, i, argv)) {
            continue;
        }
        if (get_value<long>("--seed", "-S", this->seed, i, argv)) {
            continue;
        }
        if (get_value<double>("--freq", "-F", this->frequency, i, argv)) {
            continue;
        }
        if (get_value<double>("--speed", "-s", this->speed, i, argv)) {
            continue;
        }
        if (get_value<long>("--duration", "-d", this->duration, i, argv)) {
            continue;
        }

        this->files.emplace_back(arg);
    }

    this->validate();
}


bool Args::is_arg(const char* full, const char* shortcut, const char* arg) {
    return (std::strcmp(arg, shortcut) == 0) || (std::strcmp(arg, full) == 0);
}


template<typename T>
bool Args::get_value(const char* full, const char* shortcut, T &value, int &i, char* argv[]) {
    if (std::strcmp(shortcut, argv[i]) == 0) {
        value = get_number<T>(argv[i], argv[i+1]);
        ++i;
    }
    else if (std::strncmp(full, argv[i], std::strlen(full)) == 0) {
        const char* str_value = argv[i] + (std::strlen(full) + 1);
        value = get_number<T>(argv[i], str_value);
    }
    else {
        return false;
    }

    return true;
}


template<typename T>
T Args::get_number(const char* arg, const char* value) {
    if (value == nullptr) {
        invalid_arg_exit_error(arg, "NONE PROVIDED");
    }

    errno = 0;
    char *end = nullptr;

    T result;
    if constexpr (std::is_same<T, double>()) {
        result = std::strtod(value, &end);
    }
    else if constexpr (std::is_same<T, long>()) {
        result = std::strtol(value, &end, 10);
    }

    if (errno == ERANGE) {
        std::fprintf(stderr, "%s\n", std::strerror(ERANGE));
        invalid_arg_exit_error(arg, value);
    }

    if (end == value) {
        invalid_arg_exit_error(arg, value);
    }

    return result;
}


void Args::validate() const {
    if (this->spread < 0.1) {
        std::fprintf(stderr, "Error: argument --spread must be >= 0.1.\n");
        std::exit(EXIT_FAILURE);
    }

    if (this->speed < 0.01) {
        std::fprintf(stderr, "Error: argument --speed must be >= 0.1.\n");
        std::exit(EXIT_FAILURE);
    }

    if (this->duration < 1) {
        std::fprintf(stderr, "Error: argument --duration must be >= 1.\n");
        std::exit(EXIT_FAILURE);
    }
}


void Args::invalid_arg_exit_error(const char *arg, const char *value) {
    std::fprintf(stderr, "Invalid commandline option!\n");
    std::fprintf(stderr, "Provided Argument: %s\n", arg);
    std::fprintf(stderr, "Value: %s\n", value);
    std::fprintf(stderr, "Use 'lmaocat --help' to get more infos.\n");
    std::exit(EXIT_FAILURE);
}
