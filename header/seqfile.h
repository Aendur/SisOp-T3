#ifndef SEQ_FILE_H
#define SEQ_FILE_H

#include <string>

static const int RF_HEAD_SIZE = 224;
static const int RF_TAIL_SIZE = 32;

class SeqFile {
private:
	std::string base_name;
	std::string extension;
	size_t file_size;
	char   head[RF_HEAD_SIZE + 1];
	char * file_buffer;

	unsigned long id1;
	unsigned long id2;

	void init_strings(void);
	void create_file(void);
	void create_page(size_t index, size_t max);
	void split_name(void);

public:
	SeqFile(const char * name, const char * ext, size_t size);
	~SeqFile(void);
	void write(void) const;
};

#endif

