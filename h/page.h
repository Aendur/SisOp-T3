#ifndef PAGE_H
#define PAGE_H

#include <Windows.h>

class TermUI;
class Device;
class entry;
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

	int _X0;
	int _Y0;
	bool _initialized = false;

	Mode _mode = Mode::HEX;
	bool _extended_entry_info = false;

	void print_hdr(void) const;
	void print_ftr(void) const;
	void print_adr(int nline, ULONGLONG offset) const;
	void print_hex(PBYTE line, int len) const;
	void print_hex_block(PBYTE line, int i0, int i1) const;
	void print_str(PBYTE line, int len) const;

	///
	void print_entry(bool extended) const;
	void print_short(const entry & ref, bool extended) const;
	void print_long(const entry & ref, bool extended) const;

public:
	void init(DWORD sl, DWORD cl, int x, int y);
	inline void set(PBYTE buffer, ULONGLONG offset) { _buffer = buffer; _offset = offset; }
	inline void toggle_mode(void) { _mode = (Mode)(((int)_mode + 1) % (int)Mode::MAX); }
	inline void toggle_extended(void) { _extended_entry_info = !_extended_entry_info; }
	void print(void) const;
};

#endif

