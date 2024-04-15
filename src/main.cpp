#include <array>
#include <cstdio>

#include "lmaocat/lmaocat.hpp"


static std::array<char, BUFSIZ> buffer;


auto main(const int argc, char *argv[]) -> int {
    std::setvbuf(stdout, buffer.data(), _IOFBF, BUFSIZ);
    lmaocat::main(argc, argv);
    return EXIT_SUCCESS;
}
