#ifndef PAGE_H
#define PAGE_H

#include <Windows.h>

class TermUI;
class Page {
private:
	enum class Mode {
		HEX = -1,
		CHR =  1,
	};

	TermUI * _ui = nullptr;
	PBYTE _buffer;
	DWORD _length;
	ULONGLONG _offset;

	Mode _mode = Mode::HEX;

	void print_hdr(void) const;
	void print_ftr(void) const;
	void print_adr(ULONGLONG offset) const;
	void print_hex(PBYTE line, int len) const;
	void print_hex_block(PBYTE line, int i0, int i1) const;
	void print_str(PBYTE line, int len) const;
public:
	//Page(void);
	inline void init(TermUI * t) { _ui = t; }
	inline void set(PBYTE buffer, DWORD length, ULONGLONG offset) { _buffer = buffer; _length = length; _offset = offset; }
	inline void toggle_mode(void) { _mode = (Mode)(-(int)_mode); }
	void print(bool reset) const;

	//void await(void) const;
};

#endif

