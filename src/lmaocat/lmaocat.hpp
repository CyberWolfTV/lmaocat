#ifndef LMAOCAT_HPP
#define LMAOCAT_HPP

#include <filesystem>

#include <Rainbow/Rainbow.hpp>

namespace fs = std::filesystem;


class lmaocat {
public:
    static void main(int argc, char* argv[]);

private:
    static void version();

    static void help(Rainbow &lol);

    static bool is_accessable(const fs::path &file);

    static void signal_handler(int signal);
};

#endif
