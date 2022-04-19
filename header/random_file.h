#ifndef RANDOM_FILE_H
#define RANDOM_FILE_H

#include <string>

#define RF_HEAD_SIZE 224
#define RF_TAIL_SIZE 32
#define RF_LINE_SIZE 32
#define RF_FILD_SIZE 25

class RandomFile {
private:

	char   file_name[RF_FILD_SIZE + 1];
	size_t file_size;
	size_t body_size;
	char   head[RF_HEAD_SIZE + 1];
	char   tail[RF_TAIL_SIZE + 1];
	char * body = nullptr;

	void init_head(void);
	void init_body(void);
	void init_tail(void);
public:
	RandomFile(const char * name, size_t size);
	~RandomFile(void);
	void write(void) const;
};

#endif
