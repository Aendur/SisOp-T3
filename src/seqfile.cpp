#include "seqfile.h"

#include <ctime>
#include <cstdio>
#include <cstring>

#include <string>
#include <vector>
#include <fstream>
#include <stdexcept>

using std::string;
using std::vector;

unsigned long SeqFile::gen_uuid(void) {
	std::uniform_int_distribution<unsigned long> distribution;
	return distribution(RNG);
}

const unsigned long long SeqFile::seed = std::random_device()();
std::mt19937_64 SeqFile::RNG = std::mt19937_64(seed);


#define LINE_PATH "PATH %58s\n" // len = 64
#define LINE_META "SIZE %-10llu  PART %-10llu   UID1 %08X  UID2 %08X\n" // len = 64
#define LINE_TIME "TIME %-21s %3s %32s\n"
static const int RF_HEAD_SIZE = 192;
static const int RF_LINE_SIZE =  64;
static const int RF_NAME_SIZE =  90;
static const int RF_PART_OFFS =  86;
static const int RF_MINF_SIZE = 200;


SeqFile::SeqFile(const char * name, const char * ext, size_t size, bool random) : base_name(name), extension(ext), file_size(size) {
	this->file_buffer = new char[this->file_size]; // + 1];
	
	if (!random) {
		if (base_name.length() > RF_NAME_SIZE) {
			throw std::runtime_error("File path must be at most " + std::to_string(RF_NAME_SIZE) + " characters");
		}
		if (extension.length() > 3) {
			throw std::runtime_error("File ext must be at most 3 characters");
		}
		if (file_size < RF_MINF_SIZE) {
			throw std::runtime_error("File size must be at least " + std::to_string(RF_MINF_SIZE) + " bytes");
		}

		this->id1 = gen_uuid();
		this->id2 = gen_uuid();
		this->init_strings();
		this->create_file();
	} else {
		this->init_rnd_bytes();
	}
}

void SeqFile::init_strings(void) {
	head = new char[RF_HEAD_SIZE];

	time_t now = time(NULL);
	struct tm utc;
	gmtime_s(&utc, &now);

	char time_buffer[RF_LINE_SIZE];
	snprintf(
		time_buffer        , RF_LINE_SIZE, "%5d-%02d-%02dT%02d:%02d:%02d",
		utc.tm_year + 11900, utc.tm_mon  , utc.tm_mday,
		utc.tm_hour        , utc.tm_min  , utc.tm_sec
	);

	char * pos = head;
	snprintf(pos +    0, RF_LINE_SIZE + 1, LINE_PATH , base_name.substr(0, 58).c_str());
	snprintf(pos + 0x40, RF_LINE_SIZE + 1, LINE_META, file_size, (size_t) 0, id1, id2);
	snprintf(pos + 0x80, RF_LINE_SIZE + 1, LINE_TIME, time_buffer, extension.c_str(),
		base_name.length() > 58 ? base_name.substr(58, 32).c_str() : "");

}

void SeqFile::create_file(void) {
	size_t position = 0;
	size_t index = 0;
	while (position < file_size) {
		size_t remaining =  file_size - position;
		if (remaining >= 512) {
			create_page(index++, 512);
			position += 512;
		} else {
			create_page(index++, remaining);
			position += remaining;
		}
	}
	file_buffer[file_size - 8] = '-';
	file_buffer[file_size - 7] = '-';
	file_buffer[file_size - 6] = '-';
	file_buffer[file_size - 5] = '|';
	file_buffer[file_size - 4] = 'E';
	file_buffer[file_size - 3] = 'O';
	file_buffer[file_size - 2] = 'F';
	file_buffer[file_size - 1] = '\n';
}

void SeqFile::create_page(size_t index, size_t max) {
	std::uniform_int_distribution<unsigned short> distribution('_', 'z');
	static char page[512];

	// HEAD
	memcpy(page, head, RF_HEAD_SIZE);
	snprintf(page + RF_PART_OFFS, 11, "%-10llu", index);
	page[RF_PART_OFFS + 10] = ' ';

	// BODY
	page[RF_HEAD_SIZE    ] = 'D';
	page[RF_HEAD_SIZE + 1] = 'A';
	page[RF_HEAD_SIZE + 2] = 'T';
	page[RF_HEAD_SIZE + 3] = 'A';
	page[RF_HEAD_SIZE + 4] = index == 0 ? '|' : '<';
	page[RF_HEAD_SIZE + 5] = '-';
	page[RF_HEAD_SIZE + 6] = '-';
	page[RF_HEAD_SIZE + 7] = '-';
	char * pos = page + RF_HEAD_SIZE + 8;
	char * endpos = page + 512;
	while (pos < endpos) {
		*(pos) = (char) distribution(RNG);
		++pos;
	}

	pos = page + RF_HEAD_SIZE + RF_LINE_SIZE - 1;
	while (pos < endpos) {
		*(pos) = '\n';
		pos += RF_LINE_SIZE;
	}


	// TAIL
	page[504] = '-';
	page[505] = '-';
	page[506] = '-';
	page[507] = '>';
	page[508] = 'E';
	page[509] = 'O';
	page[510] = 'S';
	page[511] = '\n';

	memcpy(file_buffer + index * 512, page, max);
}


SeqFile::~SeqFile(void) {
	delete[] this->file_buffer;
}


void SeqFile::write(void) const {
	std::ofstream stream(base_name + '.' + extension, std::ios_base::binary);
	if (stream.good()) {
		stream.write(file_buffer, file_size);
	} else {
		throw std::runtime_error("unable to open file: " + base_name + '.' + extension);
	}
	stream.close();
}

///////////

void SeqFile::init_rnd_bytes(void) {
	std::uniform_int_distribution<unsigned short> distribution(' ', '~');
	size_t n_gen = 0;
	while(n_gen < file_size) {
		file_buffer[n_gen] = (char) distribution(RNG);
		++n_gen;
	}

	static const int LW = 64;
	for (size_t i = LW-1; i < file_size; i+= LW) { file_buffer[i] = '\n'; }
	file_buffer[file_size - 1] = '\n';
}


