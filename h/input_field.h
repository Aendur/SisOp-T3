#ifndef INPUT_FIELD_H
#define INPUT_FIELD_H

#include "keycodes.h"

class TermUI;
class InputField {
private:
	
	TermUI * _term = nullptr;
	union {
		char     chr[512];
		wchar_t wchr[512];
	} _buffer;
	
	KeyCode _endkey = TERMUI_KEY_RETURN;
	static const int MESSAGE_SIZE = 64;
	char _message[MESSAGE_SIZE];
	int _position = 0;
	int _maxlen = 512;
	int _X0 = 1;
	int _Y0 = 1;
	void erase_one(void);
	bool capture_input(KeyCode feed, bool reset = true);
	void print_error(const char * msg);
public:
	void init(TermUI * t, int x0, int y0, const char * msg);
	void set_endkey(KeyCode key) { _endkey = key; }
	void set_maxlen(int len) { _maxlen = len; }

	bool get(int * out, KeyCode feed, bool keep_trying);
	bool get(char * out, KeyCode feed, bool keep_trying);
	bool get(long long * out, KeyCode feed, bool keep_trying);
	bool get(unsigned char * out, KeyCode feed, bool keep_trying);
};


#endif
