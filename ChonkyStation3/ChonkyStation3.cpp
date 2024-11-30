#include <iostream>

#include "PlayStation3.hpp"
#include "ChonkyStation3.hpp"


int main(int argc, char** argv) {
	if (argc < 2)
		Helpers::panic("Usage: ChonkyStation3.exe [executable path]");
	printf("ChonkyStation3\n");

	PlayStation3 ps3 = PlayStation3(argv[1]);

	return 0;
}
