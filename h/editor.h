#ifndef EDITOR_H
#define EDITOR_H

#include <Windows.h>

class TermUI;
class Editor {
private:
	TermUI * _term = nullptr;
	PBYTE _buffer = nullptr;
	int _position = 0;

	void move_cursor(int offset);
	
	bool edit(void);
public:
	inline void init(TermUI * t) { _term = t; }
};


#endif
