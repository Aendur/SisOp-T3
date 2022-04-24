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

#define head_template \
	"Name: %*s\n"     \
	"Size: %*zu\n"    \
	"Head: %*d\n"     \
	"Body: %*zu\n"    \
	"Tail: %*d\n"     \
	"Time: %*s\n"     \
	"---------------- START ----------------\n"

#define tail_template \
	"----------------- END -----------------\n"

#define LOG_FILE stderr

RandomFile::RandomFile(const char * name, size_t size) : file_size(size) {
	if (strlen(name) > RF_FILD_SIZE) {
		throw std::runtime_error("file_name must be at most " + std::to_string(RF_FILD_SIZE) + " characters");
	}
	snprintf(this->file_name,  RF_FILD_SIZE + 1, "%s", name);

	if (this->file_size < RF_MINF_SIZE) {
		throw std::runtime_error("file_size must be at least " + std::to_string(RF_MINF_SIZE) +" bytes");
	}
	
	this->body_size = this->file_size - RF_HEAD_SIZE - RF_TAIL_SIZE;
	this->body = new char[body_size + 1];
	int r1 = rand();
	int r2 = rand() << 8;
	int r3 = rand() << 16;
	int r4 = rand() << 24;
	this->id = r1 ^ r2 ^ r3 ^ r4;
	this->init_head();
	this->init_body();
	this->init_tail();
}

RandomFile::~RandomFile(void) {
	delete[] this->body;
}

void RandomFile::init_head(void) {
	time_t now = time(NULL);
	struct tm utc;
	gmtime_s(&utc, &now);

	char time_buffer[RF_FILD_SIZE + 1];
	snprintf(
		time_buffer        , RF_FILD_SIZE + 1, "%d-%02d-%02dT%02d:%02d:%02d",
		utc.tm_year + 11900, utc.tm_mon      , utc.tm_mday,
		utc.tm_hour        , utc.tm_min      , utc.tm_sec
	);

	snprintf(this->head, RF_HEAD_SIZE + 1, head_template,
		RF_FILD_SIZE, this->file_name,
		RF_FILD_SIZE, this->file_size,
		RF_FILD_SIZE, RF_HEAD_SIZE,
		RF_FILD_SIZE, this->body_size,
		RF_FILD_SIZE, RF_TAIL_SIZE,
		RF_FILD_SIZE, time_buffer
	);

	//delete[] time_buffer;
}
#define HYAKU_SIZE 100
#define IROHA_SIZE 600
#define HYAKUSEN \
	   "%-9llu ----:---- ----:---- ----:---- ----:---- ----:---- ----:---- ----:---- ----:---- ----:----\n"
#define IROHA \
	"IROHA   1 ----:---- ----:---- --: ・※・・・・　【　いろ は　】　・・・※・ :-- ----:---- ----:----\n" \
	"IROHA   2 ----:---- ----:---- --: ※・【　いろはにほへと ちりぬるを　】・※ :-- ----:---- ----:----\n" \
	"IROHA   3 ----:---- ----:---- --: ※・【　わかよたれそつ 　ねならむ　】・※ :-- ----:---- ----:----\n" \
	"IROHA   4 ----:---- ----:---- --: ※・【　うゐのおくやま けふこえて　】・※ :-- ----:---- ----:----\n" \
	"IROHA   5 ----:---- ----:---- --: ※・【　あさきゆめみし ゑひもせす　】・※ :-- ----:---- ----:----\n"

#define TORIN \
	"TORINAKU1 ----:---- ----:---- --: ・※・・・・　【　とり なく　】　・・※・ :-- ----:---- ----:----\n" \
	"TORINAKU2 ----:---- ----:---- --: ※・【　とりなくこゑす ゆめさませ　】・※ :-- ----:---- ----:----\n" \
	"TORINAKU3 ----:---- ----:---- --: ※・【　みよあけわたる ひんかしを　】・※ :-- ----:---- ----:----\n" \
	"TORINAKU4 ----:---- ----:---- --: ※・【　そらいろはえて おきつへに　】・※ :-- ----:---- ----:----\n" \
	"TORINAKU5 ----:---- ----:---- --: ※・【　ほふねむれゐぬ もやのうち　】・※ :-- ----:---- ----:----\n"

void RandomFile::init_body(void) {
	static char id_buffer[16];
	
	if (this->body_size == 0) {
		this->body[0] = 0;
		return;
	} else if (this->body_size == 1) {
		this->body[this->body_size - 1] = '\n';
		this->body[this->body_size] = 0;
	}

	char * pos = this->body;
	char * end = this->body + this->body_size;
	while (pos < end) {
		unsigned long long off = (unsigned long long) (RF_HEAD_SIZE + 1) + (unsigned long long) (pos - this->body);
		snprintf(pos, (size_t) (end - pos), HYAKUSEN, off);
		pos += HYAKU_SIZE;
	}

	snprintf(id_buffer, 16, "%08xh", id);
	memcpy(this->body + 20, id_buffer, 9);

	long long offset;
	if (4*IROHA_SIZE <= this->body_size) {
		offset = (long long) (this->body_size - IROHA_SIZE) / 2;
		offset -= offset % HYAKU_SIZE;
		if (id % 2 == 0) memcpy(this->body + offset, IROHA, IROHA_SIZE);
		else             memcpy(this->body + offset, TORIN, IROHA_SIZE);
		
		
	}

	this->body[this->body_size - 1] = '\n';
	this->body[this->body_size] = 0;
}

void RandomFile::init_tail(void) {
	snprintf(this->tail, RF_TAIL_SIZE + 1, tail_template);
}

void RandomFile::write(void) const {
	std::ofstream stream(this->file_name, std::ios_base::binary);
	if (stream.good()) {
		stream << this->head;
		stream << this->body;
		stream << this->tail;
	} else {
		throw std::runtime_error("unable to open file: " + string(this->file_name));
	}
	stream.close();
}


#include "random_file.h"
#include "utility.h"

#include <stdexcept>
#include <iostream>
#include <string>
#include <cstdlib>
#include <ctime>

int get_len(int x) {
	int len = 1;
	while (x > 9) {
		x /= 10;
		++len;
	}
	return len;
}

char * find_dot(char * str) {
	printf("search dot @ %p\n", str);
	++str;
	while(*str != '\0') {
		if (*str == '.') {
			printf("found dot @ %p\n", str);
			return str;
		}
		++str;
	}
	return NULL;
}

const char * get_filename(char * name, int current, int N) {
	if (N == 1) return name;
	if (strlen(name) < 2) return name;
	
	static const int BUFSIZE = 64;
	static char file_name_buffer[BUFSIZE];
	static char * ext = find_dot(name);
	if (ext != NULL) {
		*ext = 0;
		int L = get_len(N);
		snprintf(file_name_buffer, BUFSIZE, "%s%0*d.%s", name, L, current + 1, ext + 1);
		return file_name_buffer;
	} else {
		int L = get_len(N);
		snprintf(file_name_buffer, BUFSIZE, "%s%0*d", name, L, current + 1);
		return file_name_buffer;
	}

}

int main (int argc, char ** argv) {
	srand((unsigned int) time(NULL));
	if (argc < 4) {
		std::cout << "a.out file_name file_size num_files [dry_run]\n\n";
		std::cout << "      file_name max 25 characters\n";
		std::cout << "      file_size in bytes\n";
		return 0;
	}
	bool dry_run = argc == 5;
	try {
		size_t fsize = std::stoull(argv[2]);
		int max_files = std::stoi(argv[3]);
		for (int i = 0; i < max_files; ++i) {
			const char * fname = get_filename(argv[1], i, max_files);
			std::cout << "Creating file: " << fname << " - " << size_to_string(fsize, true) << "\n";
			if (!dry_run) {
				RandomFile rf(fname, fsize);
				rf.write();
			}
		}
	} catch (std::exception & e) {
		std::cout << "Error: " << e.what() << std::endl;
	}

	return 0;
}

