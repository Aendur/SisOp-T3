#ifndef EDITOR_H
#define EDITOR_H

#include <Windows.h>
#include <deque>
#include "input_field.h"
#include "layout.h"

class TermUI;
class Page;
class Device;
class Dialog;
class Editor {
private:
	enum class EditMode {
		UNK,
		HEX,
		CHR,
		STR,
	};
	enum class CursorMoveMode {
		STAY,
		HALT,
		WRAP,
	};

	enum EditorAction {
		KEEP_EDITING     = DIALOG_NO_SELECTION,
		WRITE_CHANGES   = 1,
		DISCARD_CHANGES = 2,
	};

	TermUI *   _term = nullptr;
	Page *  _page[2] = { nullptr, nullptr };
	PBYTE _buffer[2] = { nullptr, nullptr };
	LONGLONG _device_offset;
	int    _position = 0;
	DWORD _buf_len;

	EditMode _edit_mode = EditMode::UNK;
	std::deque<std::pair<LONGLONG, BYTE>> _history;

	InputField _input;
	bool _initialized = false;

	void switch_edit_mode(void);
	void print_commands(void) const;
	void print_stack(int max);
	void move_cursor(int offset, CursorMoveMode mode);
	bool set_cursor(LONGLONG newpos);

	void push_byte(unsigned char byte);
	void push_str(const char * str);
	void pop_byte(void);
	void write_changes(void);

	EditorAction edit_run(void);
	EditorAction proc_dialog(Dialog & dialog);
public:
	~Editor(void);
	void init(DWORD size, TermUI * t, Page * p1, Page * p2);
	bool edit(Device& dev);
};


#endif
