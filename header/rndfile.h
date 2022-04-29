#ifndef RND_FILE_H
#define RND_FILE_H

#include <string>
#include <cstddef>

class RndFile {
private:
	std::string file_name;
	char * file_buffer;
	size_t file_size;
	void init_bytes(void);

public:
	RndFile(const char * name, const char * ext, size_t size);
	~RndFile(void);
	void write(void) const;
};

#endif
