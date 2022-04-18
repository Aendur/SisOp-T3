#ifndef RANDOM_FILE_H
#define RANDOM_FILE_H

#include <string>

class RandomFile {
private:
	const size_t DESC_SIZE = 12;
	const size_t FILD_SIZE = 20;
	size_t head_size;
	size_t body_size;
	size_t tail_size;
	size_t file_size;
	char * file_name = nullptr;

	char * head = nullptr;
	char * body = nullptr;
	char * tail = nullptr;

	void init_head(void);
	void init_body(void);
	void init_tail(void);

	//inline size_t get_file_size(void) const { return head_}
public:
	RandomFile(const char * name, size_t size);
	~RandomFile(void);
	void Write(void) const;
};

#endif
