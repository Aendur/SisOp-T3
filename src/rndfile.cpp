#include "rndfile.h"

#include <fstream>

RndFile::RndFile(const char * name, const char * ext, size_t size) {
	this->file_name = std::string(name) + '.' + std::string(ext);
	this->file_buffer = new char[size];
	this->file_size = size;
	this->init_bytes();
}

RndFile::~RndFile(void) {
	delete[] this->file_buffer;
}

void RndFile::init_bytes(void) {
	unsigned int X;
	int match3 = 0;
	int match2 = 0;
	int match1 = 0;
	size_t n_gen = 0;
	while(n_gen < file_size - 4) {
		X = (unsigned int) rand();
		const unsigned char * C = (unsigned char*) &X;
		for (int i = 0; i < 4; ++i) {
			if (' ' <= C[i] && C[i] <= '~') {
				this->file_buffer[n_gen++] = C[i];
				++match1;
			} else {
				unsigned char R = ((C[i] & 0x0F) << 4) | ((C[i] & 0xF0) >> 4);
				if (' ' <= R && R <= '~') {
					++match2;
					this->file_buffer[n_gen++] = R;
				} else {
					++match3;
				}
			}
		}

	}
	double ntotal = match1 + match2 + match3;
	printf("%5d %.3f\n", match1, match1 / ntotal);
	printf("%5d %.3f\n", match1+match2, (match1+match2) / ntotal);
	printf("%5d %.3f\n", match3, match3 / ntotal);
	
	for (size_t i = 63; i < file_size; i+= 64) { file_buffer[i] = '\n'; }
	int last_rand[3] = { rand(), rand(), rand() };
	file_buffer[file_size - 1] = '\n';
	file_buffer[file_size - 2] = last_rand[0] % 2 == 0 ? 'N' : 'A';
	file_buffer[file_size - 3] = last_rand[1] % 2 == 0 ? 'I' : 'Y';
	file_buffer[file_size - 4] = last_rand[2] % 2 == 0 ? 'R' : 'A';

}


void RndFile::write(void) const {
	std::ofstream stream;
	stream.open(file_name, stream.binary);
	stream.write(file_buffer, file_size);
	stream.close();
}

