#ifndef PAGE_H
#define PAGE_H

#include <Windows.h>

class Page {
private:
	PBYTE _buffer;
	PBYTE _offset;
	DWORD _length;

	void print_hdr(void) const;
	void print_adr(long long offset) const;
	void print_hex(PBYTE line, int len) const;
	void print_str(PBYTE line, int len) const;
public:
	Page(void);
	void set(PBYTE _buffer, PBYTE _offset, DWORD length);
	void print(void) const;
};

#endif
