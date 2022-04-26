#include "input_field.h"
#include "layout.h"
#include "term_ui.h"

#include <cstdio>
#include <string>
#include <stdexcept>

void InputField::init(TermUI * t, int x0, int y0, const char * msg) {
	_term = t;
	_X0 = x0;
	_Y0 = y0;
	snprintf(_message, MESSAGE_SIZE, "%s", msg);
}

bool InputField::capture_input(KeyCode feed) {
	if (_term == nullptr) {
		printf(LAYOUT_FREE "input field not initialized");
		return false;
	}

	_position = 0;
	_buffer.chr[0] = 0;

	if (feed != _endkey && TERMUI_KEY_SPACE <= feed && feed <= TERMUI_KEY_TILDE) {
		_buffer.chr[_position++] = (char) feed;
		_buffer.chr[_position] = 0;
	}

	printf("\033[%d;%dH%s%s", _Y0, _X0, _message, _buffer.chr);
	KeyCode key = TERMUI_KEY_UNDEFINED;

	while (_position < _maxlen && (key = _term->read()) != _endkey && key != TERMUI_KEY_ESC) {

		if (TERMUI_KEY_SPACE <= key && key <= TERMUI_KEY_TILDE) {
			printf("%c", key);
			_buffer.chr[_position++] = (char) key;
			_buffer.chr[_position] = 0;
		} else {
			switch (key) {
			case TERMUI_KEY_BKSPC:  erase_one(); break;
			}
		}
	}
	//printf("test");
	//printf("\033[%d;%dH", _Y0, _X0);
	//printf("%*s", _position + (int) strlen(_message), "");
	
	// ERASE SEQ "\033[1K"
	//printf("\033[%d;%dH\033[1K", _Y0, _X0);
	printf("\033[1K");
	return (key == _endkey || _position >= _maxlen);
}

void InputField::erase_one(void) {
	if (_position > 0) {
		_buffer.chr[--_position] = 0;
		printf("\033[1D\033[0K");
	}
}

bool InputField::get(long long * out, KeyCode feed) {
	bool captured = capture_input(feed);
	if (captured) {
		try {
			*out = std::stoll(_buffer.chr);
		} catch (std::exception &) {
			//printf("\033[1KError: %s", e.what());
			return false;
		}
		return true;
	} else {
		return false;
	}
}

bool InputField::get(unsigned char * out, KeyCode feed) {
	bool captured = capture_input(feed);
	if (captured) {
		try {
			*out = (unsigned char) std::stoul(_buffer.chr);
		} catch (std::exception &) {
			//printf("\033[1KError: %s", e.what());
			return false;
		}
		return true;
	} else {
		return false;
	}
}
