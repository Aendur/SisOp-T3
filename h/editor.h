#ifndef EDITOR_H
#define EDITOR_H

#include <Windows.h>
#include "input_field.h"

class TermUI;
class Page;
class Device;
class Editor {
private:
	TermUI *   _term = nullptr;
	Page *  _page[2] = { nullptr, nullptr};
	PBYTE _buffer[2] = { nullptr, nullptr};
	int    _position = 0;
	DWORD _buf_len;

	InputField _input;
	bool _initialized = false;

	void move_cursor(int offset);
	void edit_start(void);
	bool edit_run(void);
public:
	~Editor(void);
	void init(DWORD size, TermUI * t, Page * p1, Page * p2);
	bool edit(const Device& dev);


};


#endif
