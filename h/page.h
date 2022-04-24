#ifndef PAGE_H
#define PAGE_H

#include <Windows.h>

class TermUI;
class Device;
class Page {
private:
	enum class Mode : int {
		HEX,
		CHR,
		CHX,
		MAX,
	};

	PBYTE _buffer;
	ULONGLONG _offset;
	
	DWORD _sector_length;
	DWORD _clustr_length;

	Mode _mode = Mode::HEX;

	void print_hdr(void) const;
	void print_ftr(void) const;
	void print_adr(ULONGLONG offset) const;
	void print_hex(PBYTE line, int len) const;
	void print_hex_block(PBYTE line, int i0, int i1) const;
	void print_str(PBYTE line, int len) const;
public:
	inline void init(DWORD sl, DWORD cl) { _sector_length = sl; _clustr_length = cl; }
	inline void set(PBYTE buffer, ULONGLONG offset) { _buffer = buffer; _offset = offset; }
	inline void toggle_mode(void) { _mode = (Mode)(((int)_mode + 1) % (int)Mode::MAX); }
	void print(void) const;
};

#endif

