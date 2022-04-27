#include "random_file.h"

#include <ctime>
#include <cstdio>
#include <cstring>

#include <string>
#include <vector>
#include <fstream>
#include <stdexcept>

using std::string;
using std::vector;

#define LOG_FILE stderr
static const int RF_LINE_SIZE = 32;
static const int RF_FILD_SIZE = 26;
static const int RF_NAME_SIZE = 94;
static const int RF_MINF_SIZE = (RF_HEAD_SIZE + RF_TAIL_SIZE + RF_LINE_SIZE);

unsigned long gen_uuid(void) {
	unsigned long r1 = rand();
	unsigned long r2 = rand();
	unsigned long r3 = rand();
	unsigned long r4 = rand();
	r1 = (r1 & 0x000000ff) ^ (r2 & 0x0000ff00 >> 8) ^ (r3 & 0x00ff0000 >> 16) ^ (r4 & 0xff000000 >> 24);
	r2 = (r2 & 0x000000ff) ^ (r3 & 0x0000ff00 >> 8) ^ (r4 & 0x00ff0000 >> 16) ^ (r1 & 0xff000000 >> 24);
	r3 = (r3 & 0x000000ff) ^ (r4 & 0x0000ff00 >> 8) ^ (r1 & 0x00ff0000 >> 16) ^ (r2 & 0xff000000 >> 24);
	r4 = (r4 & 0x000000ff) ^ (r1 & 0x0000ff00 >> 8) ^ (r2 & 0x00ff0000 >> 16) ^ (r3 & 0xff000000 >> 24);
	return r1 | (r2 << 8) | (r3 << 16) | (r4 << 24);
}

RandomFile::RandomFile(const char * name, const char * ext, size_t size) : base_name(name), extension(ext), file_size(size) {
	if (base_name.length() > RF_NAME_SIZE) {
		throw std::runtime_error("File name must be at most " + std::to_string(RF_NAME_SIZE) + " characters");
	}
	if (extension.length() > 3) {
		throw std::runtime_error("File name must be at most 3 characters");
	}
	if (this->file_size < RF_MINF_SIZE) {
		throw std::runtime_error("File size must be at least " + std::to_string(RF_MINF_SIZE) +" bytes");
	}
	
	this->file_buffer = new char[this->file_size]; // + 1];
	this->id1 = gen_uuid();
	this->id2 = gen_uuid();
	this->init_strings();
	this->create_file();
}

static const int RF_PART_OFFSET = 101;
static const char template1[] = "PATH %26s\n";
static const char template2[] = "TIME %20s %5s\n";
static const char template3[] = "SIZE %-9d %16s\n";
static const char template4[] = "PART %-7d %18s\n";
static const char template5[] = "UID1 %08x %17s\n";
static const char template6[] = "UID2 %08x %17s\n";
static const char template7[] = "---------+-- BEGIN ------------\n";
static const char template8[] = "%-9u|.........:.........:.\n";
static const char template9[] = "---------+--- END -------------\n";

void RandomFile::init_strings(void) {
	time_t now = time(NULL);
	struct tm utc;
	gmtime_s(&utc, &now);

	char time_buffer[RF_FILD_SIZE + 1];
	snprintf(
		time_buffer        , RF_FILD_SIZE + 1, "%d-%02d-%02dT%02d:%02d:%02d",
		utc.tm_year + 11900, utc.tm_mon      , utc.tm_mday,
		utc.tm_hour        , utc.tm_min      , utc.tm_sec
	);

	char * pos = head;
	snprintf(pos +    0, RF_LINE_SIZE + 1, template1, base_name.substr(0, 26).c_str());
	snprintf(pos + 0x20, RF_LINE_SIZE + 1, template2, time_buffer, extension.c_str());
	snprintf(pos + 0x40, RF_LINE_SIZE + 1, template3, file_size, base_name.length() > 26 ? base_name.substr(26, 16).c_str() : "");
	snprintf(pos + 0x60, RF_LINE_SIZE + 1, template4, 0,   base_name.length() > 42 ? base_name.substr(42, 18).c_str() : "");
	snprintf(pos + 0x80, RF_LINE_SIZE + 1, template5, id1, base_name.length() > 60 ? base_name.substr(60, 17).c_str() : "");
	snprintf(pos + 0xA0, RF_LINE_SIZE + 1, template6, id2, base_name.length() > 77 ? base_name.substr(77, 17).c_str() : "");
	snprintf(pos + 0xC0, RF_LINE_SIZE + 1, template7);
}

void RandomFile::create_file(void) {
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
	file_buffer[file_size - 5] = ':';
	file_buffer[file_size - 4] = 'E';
	file_buffer[file_size - 3] = 'O';
	file_buffer[file_size - 2] = 'F';
	file_buffer[file_size - 1] = '\n';
	//file_buffer[file_size] = 0;
}

void RandomFile::create_page(size_t index, size_t max) {
	// fprintf(LOG_FILE, "creating page %llu (max %llu)\n", index, max);
	static int nlines = (512 - (RF_HEAD_SIZE + RF_TAIL_SIZE)) / RF_LINE_SIZE;
	static char page[512];
	char * target = page;
	size_t current = index * 512;

	memcpy(target, head, RF_HEAD_SIZE);
	target += RF_HEAD_SIZE;
	current += RF_HEAD_SIZE;
	for (int i = 0; i < nlines; ++i) {
		snprintf(target, 33, template8, current);
		target += 32;
		current += 32;
	}
	memcpy(target, template9, RF_TAIL_SIZE);

	if (target + 32 != page + 512) {
		fprintf(LOG_FILE, "%p\n", target + 32);
		fprintf(LOG_FILE, "%p\n", page + 512);
		throw std::logic_error("buffer not complete");
	}

	snprintf(page + RF_PART_OFFSET, 8, "%-7llu", index);
	page[RF_PART_OFFSET + 7] = ' ';

	memcpy(file_buffer + index * 512, page, max);
}


RandomFile::~RandomFile(void) {
	delete[] this->file_buffer;
}


void RandomFile::write(void) const {
	std::ofstream stream(base_name + '.' + extension, std::ios_base::binary);
	if (stream.good()) {
		stream.write(file_buffer, file_size);
	} else {
		throw std::runtime_error("unable to open file: " + base_name + '.' + extension);
	}
	stream.close();
}

