#include "random_file.h"

#include <ctime>
#include <cstdio>
#include <cstring>

#include <string>
#include <vector>
#include <fstream>

using std::string;
using std::vector;

#define head_template \
	"Name: %*s\n"     \
	"Size: %*zu\n"    \
	"Head: %*d\n"     \
	"Body: %*zu\n"    \
	"Tail: %*d\n"     \
	"Time: %*s\n"     \
	"------------ START ------------\n"

#define tail_template \
	"------------- END -------------\n"

#define LOG_FILE stderr

RandomFile::RandomFile(const char * name, size_t size) : file_size(size) {
	if (strlen(name) > RF_FILD_SIZE) {
		throw std::runtime_error("file_name too long");
	}
	snprintf(this->file_name,  RF_FILD_SIZE + 1, "%s", name);

	if (this->file_size < RF_HEAD_SIZE + RF_TAIL_SIZE) {
		throw std::runtime_error("file_size must be greater than " + std::to_string(RF_HEAD_SIZE + RF_TAIL_SIZE));
	}
	
	this->body_size = this->file_size - RF_HEAD_SIZE - RF_TAIL_SIZE;
	this->body = new char[body_size + 1];
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
#define IROHA_SIZE 500
#define hyakusen \
	"%-9llu ----:---- ----:---- ----:---- ----:---- ----:---- ----:---- ----:---- ----:---- ----:----\n"
#define iroha \
	"                                ※・・・・・【　　　いろ は　　】・・・・・※\n" \
	"                                ※・・【　いろはにほへと ちりぬるを　】・・※\n" \
	"                                ※・・【　わかよたれそつ 　ねならむ　】・・※\n" \
	"                                ※・・【　うゐのおくやま けふこえて　】・・※\n" \
	"                                ※・・【　あさきゆめみし ゑひもせす　】・・※\n"
#define torinaku \
	"                                ※・・・・・【　　　とり なく　】・・・・・※\n" \
	"                                ※・・【　とりなくこゑす ゆめさませ　】・・※\n" \
	"                                ※・・【　みよあけわたる ひんかしを　】・・※\n" \
	"                                ※・・【　そらいろはえて おきつへに　】・・※\n" \
	"                                ※・・【　ほふねむれゐぬ もやのうち　】・・※\n"

void RandomFile::init_body(void) {
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
		snprintf(pos, (size_t) (end - pos), hyakusen, off);
		pos += HYAKU_SIZE;
	}

	long long offset;
	if (5*IROHA_SIZE <= this->body_size) {
		offset = (long long) (this->body_size - IROHA_SIZE) / 2;
		offset -= offset % HYAKU_SIZE;
		if (rand() % 2 == 0) memcpy(this->body + offset, iroha, IROHA_SIZE);
		else                 memcpy(this->body + offset, torinaku, IROHA_SIZE);
	}

	this->body[this->body_size - 1] = '\n';
	this->body[this->body_size] = 0;
}

void RandomFile::init_tail(void) {
	snprintf(this->tail, RF_TAIL_SIZE + 1, tail_template);
}

void RandomFile::write(void) const {
	std::ofstream stream(this->file_name);
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

int main (int argc, char ** argv) {
	srand((unsigned int) time(NULL));
	if (argc < 3) {
		std::cout << "a.out file_name file_size\n\n";
		std::cout << "      file_name max 25 characters\n";
		std::cout << "      file_size in bytes\n";
		return 0;
	}
	try {
		size_t fsize = std::stoull(argv[2]);
		std::cout << "Creating file: " << argv[1] << " - " << size_to_string(fsize, true) << "\n";
		RandomFile rf(argv[1], fsize);
		rf.write();
	} catch (std::exception & e) {
		std::cout << "Error: " << e.what() << std::endl;
	}

	return 0;
}

