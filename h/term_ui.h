#ifndef TERM_UI_H
#define TERM_UI_H

#include <Windows.h>
#include <utility>
#include "keycodes.h"

class TermUI {
private:
	HANDLE _handle_in = NULL;
	HANDLE _handle_out = NULL;
	HANDLE _handle_err = NULL;
	DWORD _mode0_in = (unsigned) -1;
	DWORD _mode0_out = (unsigned) -1;
	DWORD _mode0_err = (unsigned) -1;
	DWORD _mode1_in;
	DWORD _mode1_out;
	DWORD _mode1_err;
	DWORD _mode2_in = (unsigned) -1;
	DWORD _mode2_out = (unsigned) -1;
	DWORD _mode2_err = (unsigned) -1;
	void init_in(void);
	void init_out(void);
	void init_err(void);
	KeyCode handle_input(void);
	KeyCode handle_esc(void);

	inline static const int BUFSIZE = 16;
	wchar_t _input_string[BUFSIZE];
	DWORD _input_nreads;

	std::pair<int,int> _position;
public:
	TermUI(void);
	~TermUI(void);
	void init(void);
	KeyCode read(void);
	void write(const wchar_t * msg);

	void save_position(void);
	void load_position(void);
	void clear_screen(void);

	const wchar_t * str(void) const { return _input_string; }
	size_t len(void) const { return _input_nreads; }
};

#endif
