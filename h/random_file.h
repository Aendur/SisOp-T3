#ifndef RANDOM_FILE_H
#define RANDOM_FILE_H

#include <string>

static const int RF_HEAD_SIZE = 224;
static const int RF_TAIL_SIZE = 32;

class RandomFile {
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
	RandomFile(const char * name, const char * ext, size_t size);
	~RandomFile(void);
	void write(void) const;
};

#endif
