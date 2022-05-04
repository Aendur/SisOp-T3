#ifndef PAGE_H
#define PAGE_H

#include <Windows.h>

#include <map>

class TermUI;
class Device;
struct entry;
struct fat32;
class Page {
private:
	enum class View : int {
		SECTORS,
		ENTRIES,

		SECTORS_BEG = 0x11,
		SECTORS_HEX,
		SECTORS_ASC,
		SECTORS_INT,
		SECTORS_END,
		
		ENTRIES_BEG = 0x21,
		ENTRIES_SHO,
		ENTRIES_LON,
		ENTRIES_END,

		SECTORS_LEN = SECTORS_END - SECTORS_BEG - 1,
		ENTRIES_LEN = ENTRIES_END - ENTRIES_BEG - 1,
		SECTORS_INI = SECTORS_BEG + 1,
		ENTRIES_INI = ENTRIES_BEG + 1,
	};
	struct FatStat {
		int  num;
		int  index;
		bool is_at_fat;
	};

	PBYTE _buffer;
	ULONGLONG _offset_end;

	PBYTE _buffers[2];
	int _selected_buffer = 0;
	
	DWORD _sector_length;
	DWORD _clustr_length;
	LONGLONG _fds_offset;
	fat32 * _sector0;

	int _X0;
	int _Y0;
	bool _initialized = false;
	int _selected = -1;
	bool _editing = false;

	std::map<View, View> _view;
	View _current_view;
	bool _show_info_text = true;

	void print_sector(void) const;
	void print_sector_int(void) const;
	void print_adr(int nline, ULONGLONG offset) const;
	void print_hex(PBYTE line) const;
	void print_int(PBYTE line) const;
	void print_hex_block(PBYTE line, int i0, int i1) const;
	void print_int_block(UINT32 value, bool selected, FatStat fati) const;
	void print_sector_bytes(PBYTE line) const;
	void print_sector_text(PBYTE line, int len) const;
	void print_sector_str(PBYTE line, int len) const;
	void print_fat_info(const FatStat & fat, UINT32 val, int nline) const;

	LONGLONG current_cluster(void) const;
	LONGLONG current_sector(void) const;

	static const char * int_to_bytestr(UINT32 x);
	static const char * val_to_remark(const FatStat & fat, UINT32 val, int i);

	///
	void print_entry(void) const;
	void print_short(const entry & ref, bool extended, bool selected) const;
	void print_long(const entry & ref, bool extended, bool selected) const;

	int  offset_selection(int i) const;
	bool actual_selected_byte(int i) const;
	bool actual_selected_entry(int i) const;
	bool actual_selected_int(int i) const;
	FatStat fat_stat(int i) const;

public:
	void init(fat32 * f32, int x, int y);
	void set(const PBYTE buffers[2], ULONGLONG offset);
	//inline void toggle_view(void) { _view = (View)((1 + (int)_view) % (int)View::MOD);  }
	inline void cycle_entries_views(void);
	inline void cycle_sectors_views(void);
	inline void toggle_edit(bool val) { _editing = val; }
	inline void switch_buff(void) { _selected_buffer = (_selected_buffer + 1) % 2; _buffer = _buffers[_selected_buffer]; }
	inline void switch_text(void) { _show_info_text = !_show_info_text; }
	inline void select(int p) { _selected = p; }
	inline int selected(void) const  { return _selected; }
	inline ULONGLONG offset_start(void) const { return _offset_end - _sector_length * (2 - _selected_buffer); }
	void print(void) const;
};

void Page::cycle_entries_views(void) {
	if  (_current_view == View::ENTRIES) _view[View::ENTRIES] = (View)((1 + (int)_view[View::ENTRIES]) % (int) View::ENTRIES_LEN + (int) View::ENTRIES_INI);
	else _current_view  = View::ENTRIES; 
}
void Page::cycle_sectors_views(void) {
	if  (_current_view == View::SECTORS) _view[View::SECTORS] = (View)((1 + (int)_view[View::SECTORS]) % (int) View::SECTORS_LEN + (int) View::SECTORS_INI);
	else _current_view  = View::SECTORS; 
}

#endif

