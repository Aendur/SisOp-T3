#ifndef PAGE_H
#define PAGE_H

#include <minwindef.h>

class Page {
private:
	void print_hdr(void) const;
	void print_adr(long long offset) const;
	void print_hex(PBYTE line, int len) const;
	void print_str(PBYTE line, int len) const;
public:
	void print_buf(PBYTE buffer, DWORD len, DWORD nbytes) const;
};

#endif
