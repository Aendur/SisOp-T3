#ifndef EDITOR_H
#define EDITOR_H

#include <Windows.h>
#include "input_field.h"

class TermUI;
class Page;
class Editor {
private:
	TermUI * _term = nullptr;
	Page * _page = nullptr;
	PBYTE _buffer = nullptr;
	int _position = 0;

	InputField _input;
	bool _initialized = false;

	void move_cursor(int offset);
	bool edit(void);
public:
	void init(TermUI * t, Page * p);

};


#endif
