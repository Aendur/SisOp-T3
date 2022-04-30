#ifndef SEQ_FILE_H
#define SEQ_FILE_H

#include <string>
#include <random>

class SeqFile {
private:
	static const unsigned long long seed;
	static std::mt19937_64 RNG;

	std::string base_name;
	std::string extension;
	size_t file_size;
	char * file_buffer;

	unsigned long id1;
	unsigned long id2;
	char * head = nullptr;
	char * tail = nullptr;

	unsigned long gen_uuid(void);
	void init_rnd_bytes(void);
	void init_strings(void);
	void create_file(void);
	void create_page(size_t index, size_t max);
	void split_name(void);

public:
	SeqFile(const char * name, const char * ext, size_t size, bool random);
	~SeqFile(void);
	void write(void) const;
};

#endif

