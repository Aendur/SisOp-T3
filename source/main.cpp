#include "random_file.h"

#include <stdexcept>
#include <iostream>

int main (int argc, char ** argv) {
	RandomFile f1("test1.txt", 256);
	RandomFile f2("test2.txt", 512);
}

