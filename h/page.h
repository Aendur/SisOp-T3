#ifndef PAGE_H
#define PAGE_H

#include <Windows.h>

#include <map>

class TermUI;
class Device;
class entry;
class Page {
private:
	enum class View : int {
		SECTOR,
		ENTRIES,
	};

	PBYTE _buffer;
	ULONGLONG _offset;
	
	DWORD _sector_length;
	DWORD _clustr_length;

	int _X0;
	int _Y0;
	bool _initialized = false;

	std::map<View, BYTE> _mode;
	View _view = View::SECTOR;

	void print_sector(void) const;
	void print_adr(int nline, ULONGLONG offset) const;
	void print_hex(PBYTE line, int len) const;
	void print_hex_block(PBYTE line, int i0, int i1) const;
	void print_sector_str(PBYTE line, int len) const;

	///
	void print_entry(void) const;
	void print_short(const entry & ref, bool extended) const;
	void print_long(const entry & ref, bool extended) const;

public:
	void init(DWORD sl, DWORD cl, int x, int y);
	inline void set(PBYTE buffer, ULONGLONG offset) { _buffer = buffer; _offset = offset; }
	inline void toggle_mode(void) { ++_mode[_view]; }
	inline void toggle_view(void) { _view = _view == View::SECTOR ? View::ENTRIES : View::SECTOR;
		// printf("\033[48;1H%d\n", _view);
		// printf("%d\n", _view);
		// printf("%d\n", _view);
		// printf("%d\n", _view);
		// printf("%d\n", _view);
	 } // = (View) (((int)_view + 1) % (int) View::MAX); }
	void print(void) const;
};

#endif

