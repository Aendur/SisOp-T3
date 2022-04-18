#include "random_file.h"

#include <ctime>
#include <cstdio>
#include <cstring>

#include <string>
#include <vector>
#include <fstream>

using std::string;
using std::vector;

static const vector<const char*> head_template = {
	"File name: %*s\n", // 12 + N
	"File size: %*d\n", // 12 + N
	"Head size: %*d\n", // 12 + N
	"Body size: %*d\n", // 12 + N
	"Tail size: %*d\n", // 12 + N
	"Timestamp: %*s\n", // 12 + N
	"-- START --\n"     // 12
};

#define LOG_FILE stderr

RandomFile::RandomFile(const char * name, size_t size) : file_size(size) {
	this->file_name = new char[FILD_SIZE + 1];
	snprintf(this->file_name,  FILD_SIZE + 1, name);

	this->tail_size = this->DESC_SIZE;
	this->head_size = this->DESC_SIZE + (this->DESC_SIZE + this->FILD_SIZE) * (head_template.size() - 1);
	this->body_size = this->file_size -  this->tail_size - this->head_size;

	if (this->file_size < this->head_size + this->tail_size) {
		throw std::runtime_error("file_size must be greater than " + std::to_string(this->head_size + this->tail_size));
	}

	fprintf(LOG_FILE, "this->file_name %s\n" , this->file_name);
	fprintf(LOG_FILE, "this->file_size %lu\n", this->file_size);
	fprintf(LOG_FILE, "this->head_size %lu\n", this->head_size);
	fprintf(LOG_FILE, "this->body_size %lu\n", this->body_size);
	fprintf(LOG_FILE, "this->tail_size %lu\n", this->tail_size);
	
	this->head = new char[head_size + 1];
	this->body = new char[body_size + 1];
	this->tail = new char[tail_size + 1];

	this->init_head();
	this->init_body();
	this->init_tail();

	fprintf(LOG_FILE, "this->head:\n%s\n", this->head);
	fprintf(LOG_FILE, "this->body:\n%s\n", this->body);
	fprintf(LOG_FILE, "this->tail:\n%s\n", this->tail);
}

RandomFile::~RandomFile(void) {
	delete[] this->file_name;
	delete[] this->head;
	delete[] this->body;
	delete[] this->tail;
}

void RandomFile::init_head(void) {
	size_t N = DESC_SIZE + FILD_SIZE;
	
	time_t now = time(NULL);
	struct tm * utc = gmtime(&now);
	char * time_buffer = new char[FILD_SIZE + 1];
	snprintf(time_buffer, FILD_SIZE + 1, "%d-%02d-%02dT%02d:%02d:%02d",
		utc->tm_year + 11900, utc->tm_mon, utc->tm_mday,
		utc->tm_hour        , utc->tm_min, utc->tm_sec
	);
	printf("%d %d\n", N, FILD_SIZE);

	snprintf(this->head + 0 * N,         N, head_template[0], FILD_SIZE, this->file_name);
	snprintf(this->head + 1 * N,         N, head_template[1], FILD_SIZE, this->file_size);
	snprintf(this->head + 2 * N,         N, head_template[2], FILD_SIZE, this->head_size);
	snprintf(this->head + 3 * N,         N, head_template[3], FILD_SIZE, this->body_size);
	snprintf(this->head + 4 * N,         N, head_template[4], FILD_SIZE, this->tail_size);
	snprintf(this->head + 5 * N,         N, head_template[5], FILD_SIZE, time_buffer);
	snprintf(this->head + 6 * N, DESC_SIZE, head_template[6]);

	delete[] time_buffer;
	fprintf(LOG_FILE, "this->head:\n%s\n", this->head);
}

void RandomFile::init_body(void) {
	memset(this->body, '.', this->body_size);
	for (int i = 20; i < this->body_size; i += 21) { this->body[i] = '\n'; }
	this->body[this->body_size - 1] = 0;
	this->body[this->body_size - 2] = '\n';
}

void RandomFile::init_tail(void) {
	snprintf(this->tail, DESC_SIZE, "--- END ---");
}

void RandomFile::Write(void) const {
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
