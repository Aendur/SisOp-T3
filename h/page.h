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
	int _selected = -1;
	bool _editing = false;

	std::map<View, BYTE> _mode;
	View _view = View::SECTOR;

	void print_sector(void) const;
	void print_adr(int nline, ULONGLONG offset) const;
	void print_hex(PBYTE line, int len) const;
	void print_hex_block(PBYTE line, int i0, int i1) const;
	void print_sector_str(PBYTE line, int len) const;

	///
	void print_entry(void) const;
	void print_short(const entry & ref, bool extended, bool selected) const;
	void print_long(const entry & ref, bool extended, bool selected) const;

public:
	void init(DWORD sl, DWORD cl, int x, int y);
	inline void set(PBYTE buffer, ULONGLONG offset) { _buffer = buffer; _offset = offset; }
	inline void toggle_mode(void) { ++_mode[_view]; }
	inline void toggle_view(void) { _view = _view == View::SECTOR ? View::ENTRIES : View::SECTOR; }
	inline void toggle_edit(bool val) { _editing = val; }
	inline void select(int p) { _selected = p; }
	inline int selected(void) const  { return _selected; }
	void print(void) const;
};

#endif

