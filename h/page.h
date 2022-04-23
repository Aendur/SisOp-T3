#ifndef PAGE_H
#define PAGE_H

#include <Windows.h>

class TermUI;
class Page {
private:
	TermUI * _ui = nullptr;
	PBYTE _buffer;
	DWORD _length;
	ULONGLONG _offset;

	void print_hdr(void) const;
	void print_ftr(void) const;
	void print_adr(ULONGLONG offset) const;
	void print_hex(PBYTE line, int len) const;
	void print_str(PBYTE line, int len) const;
public:
	//Page(void);
	inline void init(TermUI * t) { _ui = t; }
	inline void set(PBYTE buffer, DWORD length, ULONGLONG offset) { _buffer = buffer; _length = length; _offset = offset; }
	void print(bool reset) const;
	//void await(void) const;
};

#endif

