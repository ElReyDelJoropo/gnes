#include "Emulator.hpp"

int main(int argc, char **argv){
    gnes::Emulator e;
    e.run(argv[1]);
    return 0;
}
