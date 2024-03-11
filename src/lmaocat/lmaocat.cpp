#include "lmaocat.hpp"

#include <locale>
#include <cstdio>
#include <csignal>
#include <fstream>
#include <iostream>
#include <filesystem>

#include <unistd.h>

#include "Args/Args.hpp"
#include "Rainbow/Rainbow.hpp"
#include "Rainbow/ANSI/ANSI.hpp"

namespace fs = std::filesystem;


#define N L"\n"


void lmaocat::main(const int argc, char* argv[]) {
    const Args args(argc, argv);

    if (args.version) {
        version();
        std::exit(EXIT_SUCCESS);
    }

    std::signal(SIGINT,  signal_handler);
    std::signal(SIGKILL, signal_handler);
    std::signal(SIGTERM, signal_handler);

    Rainbow lol(args);

    if (args.help) {
        help(lol);
        std::exit(EXIT_SUCCESS);
    }

    std::locale::global(std::locale(""));

    bool done_cin = false;
    for (const fs::path &file: args.files) {
        if ((file.string() == "-") && !done_cin) {
            done_cin = true;
            lol.cat(std::wcin);
        }
        else {
            if (is_accessable(file)) {
                std::wifstream stream(file);
                lol.cat(stream);
                stream.close();
            }
        }
    }

    if (args.files.empty()) {
        lol.cat(std::wcin);
    }
}


void lmaocat::version() {
    std::wprintf(L"lmaocat 0.420 | (GitHub) @CyberWolfTV\n");
}


void lmaocat::help(Rainbow &lol) {
    const std::wstring help_msg{
        L"Usage: lmaocat [OPTION/s] [FILE/s]" N
        N
        "Concatenate FILE/s, or standard input, to standard output." N
        "With no FILE, or when FILE is \"-\", read standard input." N
        N
        "Options: (<f> -> decimal number, <i> -> integer)" N
        "  Info" N
        "  -h,     --help          Show this message." N
        "  -v,     --version       Print version." N
        "  Rainbow" N
        "  -p <f>, --spread=<f>    Rainbow spread (default: 3.0, min: 0.1)." N
        "  -F <f>, --freq=<f>      Rainbow frequency (default: 0.1)." N
        "  -S <i>, --seed=<i>      Rainbow seed, 0 = random (default: 0)." N
        "  -i,     --invert        Paints the background instead of the text." N
        "  -t,     --truecolor     24-bit (truecolor), even if COLORTERM is not set." N
        "  -f,     --force         Force color even if stdout is not a tty." N
        "  Animation" N
        "  -a,     --animate       Enable psychedelics." N
        "  -d <i>, --duration=<i>  Animation duration (default: 12, min: 1)." N
        "  -s <f>, --speed=<f>     Animation speed (default: 20.0, min: 0.1)." N
        N
        "Examples:" N
        "  lmaocat f - g       Output f's contents, then stdin, then g's contents." N
        "  lmaocat             Copy standard input to standard output." N
        "  neofetch | lmaocat  Make neofetch look even better!" N
        N
    };

    std::wistringstream help_stream{help_msg};
    lol.cat(help_stream);
}


bool lmaocat::is_accessable(const fs::path &file) {
    const     bool has_permissions = access(file.c_str(), R_OK) == 0;
    constexpr char perm_err[] = "Error: Permission denied! Cannot access this file: \"%s\"\n";

    try {
        if (!fs::is_regular_file(file)) {
            std::fprintf(stderr, "Error: File does not exist:\"%s\"\n", file.c_str());
            return false;
        }
    }
    catch (std::exception &e) {
        if (!has_permissions) {
            std::fprintf(stderr, perm_err, file.c_str());
        }
        else {
            std::fprintf(stderr, "Error: Unknown Exception! Cannot access this file: \"%s\"\n", file.c_str());
            std::fprintf(stderr, "%s\n", e.what());
        }
        return false;
    }

    if (!has_permissions) {
        std::fprintf(stderr, perm_err, file.c_str());
        return false;
    }

    return true;
}


void lmaocat::signal_handler(const int signal) {
    ANSI::reset_ansi_in_terminal();
    std::exit(128 + signal);
}
