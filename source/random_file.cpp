#include "random_file.h"

#include <ctime>
#include <cstdio>
#include <cstring>

#include <string>
#include <vector>
#include <fstream>

using std::string;
using std::vector;

static const char head_template[] =
	"File name: %*s\n"
	"File size: %*lu\n"
	"Head size: %*d\n"
	"Body size: %*lu\n"
	"Tail size: %*d\n"
	"Timestamp: %*s\n"
	"------------ START ------------\n"
;

static const char tail_template[] =
	"------------- END -------------\n"
;

#define LOG_FILE stderr

RandomFile::RandomFile(const char * name, size_t size) : file_size(size) {
	if (strlen(name) > RF_FILD_SIZE) {
		throw std::runtime_error("file_name too long");
	}
	snprintf(this->file_name,  RF_FILD_SIZE + 1, name);

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
	struct tm * utc = gmtime(&now);
	char * time_buffer = new char[RF_FILD_SIZE + 1];
	snprintf(
		time_buffer         , RF_FILD_SIZE + 1, "%d-%02d-%02dT%02d:%02d:%02d",
		utc->tm_year + 11900, utc->tm_mon     , utc->tm_mday,
		utc->tm_hour        , utc->tm_min     , utc->tm_sec
	);

	snprintf(this->head, RF_HEAD_SIZE + 1, head_template,
		RF_FILD_SIZE, this->file_name,
		RF_FILD_SIZE, this->file_size,
		RF_FILD_SIZE, RF_HEAD_SIZE,
		RF_FILD_SIZE, this->body_size,
		RF_FILD_SIZE, RF_TAIL_SIZE,
		RF_FILD_SIZE, time_buffer
	);

	delete[] time_buffer;
}
#define HYAKU_SIZE 100
#define IROHA_SIZE 400
char hyakusen[] =
	    "%-9lu ----:---- ----:---- ----:---- ----:---- ----:---- ----:---- ----:---- ----:---- ----:----\n";
char iroha[] =
	"                                ※・・【　いろはにほへと ちりぬるを　】・・※\n"
	"                                ※・・【　わかよたれそつ 　ねならむ　】・・※\n"
	"                                ※・・【　うゐのおくやま けふこえて　】・・※\n"
	"                                ※・・【　あさきゆめみし ゑひもせす　】・・※\n"
;
char torinaku[] =
	"                                ※・・【　とりなくこゑす ゆめさませ　】・・※\n"
	"                                ※・・【　みよあけわたる ひんかしを　】・・※\n"
	"                                ※・・【　そらいろはえて おきつへに　】・・※\n"
	"                                ※・・【　ほふねむれゐぬ もやのうち　】・・※\n"
;

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
		snprintf(pos, end - pos, hyakusen, 1 + RF_HEAD_SIZE + pos - this->body);
		pos += HYAKU_SIZE;
	}

	int offset;
	if (2*IROHA_SIZE <= this->body_size && this->body_size < 3*IROHA_SIZE) {
		offset = (this->body_size - IROHA_SIZE) / 2;
		offset -= offset % HYAKU_SIZE;
		memcpy(this->body + offset, iroha, IROHA_SIZE);
	} else if (this->body_size >= 3*IROHA_SIZE) {
		offset = (this->body_size - IROHA_SIZE * 2) / 3;
		offset -= offset % HYAKU_SIZE;
		memcpy(this->body + offset, iroha, IROHA_SIZE);
		offset += offset + IROHA_SIZE + HYAKU_SIZE;
		memcpy(this->body + offset, torinaku, IROHA_SIZE);
	}

	/*if (POEM_SIZE + 2*GOJU_SIZE <= this->body_size && this->body_size < 2*POEM_SIZE + 3*GOJU_SIZE) {
		int offset = (this->body_size - POEM_SIZE) / 2;
		offset -= offset % GOJU_SIZE;
		memcpy(this->body + offset, iroha, POEM_SIZE);
	} else if (this->body_size >= 2*POEM_SIZE + 3*GOJU_SIZE) {
		int offset = (this->body_size - POEM_SIZE * 2) / 3;
		offset -= offset % GOJU_SIZE;
		memcpy(this->body + offset, iroha, POEM_SIZE);
		memcpy(this->body + offset*2 + POEM_SIZE, torinaku, POEM_SIZE);
	}*/

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
