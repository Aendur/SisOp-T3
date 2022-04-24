#ifndef TERM_UI_H
#define TERM_UI_H

#include <Windows.h>
#include <utility>

enum KeyCode {
	TERMUI_KEY_UNDEFINED,
	TERMUI_KEY_RETURN,
	TERMUI_KEY_TAB,
	TERMUI_KEY_ARROW_UP,
	TERMUI_KEY_ARROW_RIGHT,
	TERMUI_KEY_ARROW_DOWN,
	TERMUI_KEY_ARROW_LEFT,
	TERMUI_KEY_HOME,
	TERMUI_KEY_END,
	TERMUI_KEY_PGUP,
	TERMUI_KEY_PGDOWN,
	TERMUI_KEY_SHIFT_PGUP,
	TERMUI_KEY_SHIFT_PGDOWN,
	TERMUI_KEY_PLUS,
	TERMUI_KEY_MINUS,
	TERMUI_KEY_0,
	TERMUI_KEY_1,
	TERMUI_KEY_2,
	TERMUI_KEY_3,
	TERMUI_KEY_4,
	TERMUI_KEY_5,
	TERMUI_KEY_6,
	TERMUI_KEY_7,
	TERMUI_KEY_8,
	TERMUI_KEY_9,
	TERMUI_KEY_SPACE,
	TERMUI_KEY_D,
	TERMUI_KEY_E,
	TERMUI_KEY_F,
	TERMUI_KEY_Q,
};

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
