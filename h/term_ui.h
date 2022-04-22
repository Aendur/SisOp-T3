#ifndef TERM_UI_H
#define TERM_UI_H

#include <Windows.h>

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
	void handle_input(void);

	inline static const int BUFSIZE = 16;
	TCHAR _input_string[BUFSIZE];
	DWORD _input_nreads;
public:
	TermUI(void);
	~TermUI(void);
	void init(void);
	const TCHAR * read(void);
	const TCHAR * str(void) const { return _input_string; }
	size_t len(void) const { return _input_nreads; }
};

#endif
