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

#define LINE_NAME "NAME %-23s%3s\n" // len = 32
#define LINE_META "SIZE %-10llu PART %10llu\n"// len = 32
#define LINE_UIDS "UID1 %08X   UID2   %08X\n"  // len = 32
#define LINE_TIME "%s" // len = 32


const unsigned long long SeqFile::seed = std::random_device()();
std::mt19937_64 SeqFile::RNG = std::mt19937_64(seed);

SeqFile::Header SeqFile::check_file(const unsigned char * memsrc) {
	return Header();
}

unsigned long SeqFile::gen_uuid(void) {
	std::uniform_int_distribution<unsigned long> distribution;
	return distribution(RNG);
}

SeqFile::SeqFile(const char * name, const char * ext, size_t size, bool random) : base_name(name), extension(ext), file_size(size) {
	this->file_buffer = new char[this->file_size]; // + 1];
	
	if (!random) {
		if (base_name.length() > RF_NAME_SIZE) {
			throw std::runtime_error("File name must be at most " + std::to_string(RF_NAME_SIZE) + " characters");
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
		size_t min_size = base_name.length() + extension.length() + 2;
		if (file_size < min_size) {
			throw std::runtime_error("File size must be at least " + std::to_string(min_size) + " bytes");
		}
		this->init_rnd_bytes();
	}
}

void SeqFile::init_strings(void) {
	head = new char[RF_HEAD_SIZE];

	time_t now = time(NULL);
	struct tm utc;
	gmtime_s(&utc, &now);

	char time_buffer[RF_LINE_SIZE + 1];
	snprintf(
		time_buffer        , RF_LINE_SIZE + 1, "DATE %5d-%02d-%02d TIME %02d:%02d:%02dZ\n",
		utc.tm_year + 11900, utc.tm_mon      , utc.tm_mday,
		utc.tm_hour        , utc.tm_min      , utc.tm_sec
	);

	char * pos = head;
	snprintf(pos + RF_LINE_SIZE * 0, RF_LINE_SIZE + 1, LINE_NAME , base_name.c_str(),  extension.c_str());
	snprintf(pos + RF_LINE_SIZE * 1, RF_LINE_SIZE + 1, LINE_META, file_size, (size_t) 0);
	snprintf(pos + RF_LINE_SIZE * 2, RF_LINE_SIZE + 1, LINE_UIDS, id1, id2);
	snprintf(pos + RF_LINE_SIZE * 3, RF_LINE_SIZE + 1, LINE_TIME, time_buffer);

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
	file_buffer[file_size - 7] = '-';
	file_buffer[file_size - 6] = '-';
	file_buffer[file_size - 5] = '|';
	file_buffer[file_size - 4] = 'E';
	file_buffer[file_size - 3] = 'O';
	file_buffer[file_size - 2] = 'F';
	file_buffer[file_size - 1] = '\n';
}

void SeqFile::create_page(size_t index, size_t max) {
	std::uniform_int_distribution<unsigned short> distribution('a', 'z');
	static char page[512];

	// HEAD
	memcpy(page, head, RF_HEAD_SIZE);
	snprintf(page + RF_PART_OFF, 11, "%10llu", index);
	page[RF_PART_OFF + 10] = '\n';

	// BODY
	page[RF_HEAD_SIZE    ] = 'D';
	page[RF_HEAD_SIZE + 1] = 'A';
	page[RF_HEAD_SIZE + 2] = 'T';
	page[RF_HEAD_SIZE + 3] = 'A';
	page[RF_HEAD_SIZE + 4] = index == 0 ? '|' : '<';
	page[RF_HEAD_SIZE + 5] = '-';
	page[RF_HEAD_SIZE + 6] = '-';
	char * pos = page + RF_HEAD_SIZE + 7;
	char * endpos = page + 512;
	while (pos < endpos) {
		*(pos) = (char) distribution(RNG);
		++pos;
	}

	static const int data_lwidth = 2*RF_LINE_SIZE;
	//static const int data_lwidth = 50;
	pos = page + RF_HEAD_SIZE + data_lwidth - 1;
	while (pos < endpos) {
		*(pos) = '\n';
		pos += data_lwidth;
	}


	// TAIL
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

	for (size_t i = 2*RF_LINE_SIZE-1; i < file_size; i+= 2*RF_LINE_SIZE) { file_buffer[i] = '\n'; }
	size_t len1 = base_name.length();
	size_t len2 = extension.length();

	for (char * pos = file_buffer; pos < file_buffer + file_size; pos += 512) {
		memcpy(pos, base_name.c_str(), len1);
		*(pos+len1) = '.';
		memcpy(pos+len1+1, extension.c_str(), len2);
		*(pos+len1+len2+1) = ' ';
	}


	file_buffer[file_size - 1] = '\n';
}

