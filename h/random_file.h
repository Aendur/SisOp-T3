#ifndef RANDOM_FILE_H
#define RANDOM_FILE_H

#include <string>

static const int RF_LABL_SIZE = 5;
static const int RF_FILD_SIZE = 33;
static const int RF_LINE_SIZE = RF_LABL_SIZE + RF_FILD_SIZE + 2;
static const int RF_HEAD_SIZE = RF_LINE_SIZE * 7;
static const int RF_TAIL_SIZE = RF_LINE_SIZE;
static const int RF_MINF_SIZE = (RF_HEAD_SIZE + RF_TAIL_SIZE + RF_LINE_SIZE);

#pragma warning(disable:4820)
class RandomFile {
private:
	char   file_name[RF_FILD_SIZE + 1];
	size_t file_size;
	size_t body_size;
	char * body = nullptr;
	char   head[RF_HEAD_SIZE + 1];
	char   tail[RF_TAIL_SIZE + 1];
	long   id;

	void init_head(void);
	void init_body(void);
	void init_tail(void);
public:
	RandomFile(const char * name, size_t size);
	~RandomFile(void);
	void write(void) const;
};
#pragma warning(default:4820)

#endif
