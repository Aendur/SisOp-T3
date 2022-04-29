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
		SECTORS_ASC,
		SECTORS_HEX,
		ENTRIES_SHO,
		ENTRIES_LON,
		MOD,
	};

	PBYTE _buffer;
	ULONGLONG _offset_end;

	PBYTE _buffers[2];
	int _selected_buffer = 0;
	
	DWORD _sector_length;
	DWORD _clustr_length;
	LONGLONG _fds_offset;

	int _X0;
	int _Y0;
	bool _initialized = false;
	int _selected = -1;
	bool _editing = false;

	//std::map<View, BYTE> _mode;
	View _view = View::SECTORS_ASC;

	void print_sector(void) const;
	void print_adr(int nline, ULONGLONG offset) const;
	void print_hex(PBYTE line, int len) const;
	void print_hex_block(PBYTE line, int i0, int i1) const;
	void print_sector_str(PBYTE line, int len) const;

	///
	void print_entry(void) const;
	void print_short(const entry & ref, bool extended, bool selected) const;
	void print_long(const entry & ref, bool extended, bool selected) const;

	bool actual_selected_byte(int i) const;
	bool actual_selected_entry(int i) const;

public:
	void init(DWORD sl, DWORD cl, LONGLONG fds, int x, int y);
	//inline void set(PBYTE * buffers, ULONGLONG offset) { _buffers[0] = buffers[0]; _buffers[1] = buffers[1]; _buffer = _buffers[_selected]; _offset = offset; }
	void set(const PBYTE buffers[2], ULONGLONG offset);
	//inline void toggle_mode(void) { ++_mode[_view]; }
	inline void toggle_view(void) { _view = (View)((1 + (int)_view) % (int)View::MOD);  }
	inline void toggle_edit(bool val) { _editing = val; }
	inline void switch_buff(void) { _selected_buffer = (_selected_buffer + 1) % 2; _buffer = _buffers[_selected_buffer]; }
	inline void select(int p) { _selected = p; }
	inline int selected(void) const  { return _selected; }
	inline ULONGLONG offset_start(void) const { return _offset_end - _sector_length * (2 - _selected_buffer); }
	void print(void) const;
};

#endif

