#include "random_file.h"

#include <stdexcept>
#include <iostream>

//int main (int argc, char ** argv) {
int main (int argc, char ** argv) {
	if (argc > 2) {
		RandomFile rf(argv[1], std::stoull(argv[2]));
		rf.write();
	}
	/*RandomFile rfs[] = {
		RandomFile("test001.txt", 600),
		//RandomFile("test257.txt", 257),
		//RandomFile("test257.txt", 258),
		//RandomFile("test512.txt", 512),
		//RandomFile("test01K.txt", 1024),
		//RandomFile("test02K.txt", 2048),
		//RandomFile("test04K.txt", 4096),
	};

	const int siz = 7;
	for(int i = 0; i < siz; ++i) {
		rfs[i].write();
	}*/
}

