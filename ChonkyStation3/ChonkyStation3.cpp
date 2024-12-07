#include <iostream>

#include "PlayStation3.hpp"
#include "ChonkyStation3.hpp"


int main(int argc, char** argv) {
    if (argc < 2)
        Helpers::panic("Usage: ChonkyStation3.exe [executable path]");
    printf("ChonkyStation3\n\n");

    PlayStation3 ps3 = PlayStation3(argv[1]);

    printf("\nEXECUTING\n");
    printf(  "---------\n\n");

    while (true) {
        ps3.step();
    }

    return 0;
}
