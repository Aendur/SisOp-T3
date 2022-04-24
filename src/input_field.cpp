#include "input_field.h"
#include "layout.h"
#include "term_ui.h"

#include <cstdio>
#include <string>
#include <stdexcept>

#define ERASE_SEQ "\033[1K"

bool InputField::capture_input(void) {
	if (_term == nullptr) {
		printf(LAYOUT_FREE "input field not initialized");
		return false;
	}
	_position = 0;
	_buffer.chr[0] = 0;

	printf(LAYOUT_FREE "INPUT: ");
	KeyCode key = TERMUI_KEY_UNDEFINED;

	while (true) {
		key = _term->read();

		if (TERMUI_KEY_SPACE <= key && key <= TERMUI_KEY_TILDE) {
			printf("%c", key);
			_buffer.chr[_position++] = (char) key;
			_buffer.chr[_position] = 0;
		} else {
			switch (key) {
			case TERMUI_KEY_ESC:    printf(ERASE_SEQ); return false;
			case TERMUI_KEY_RETURN: printf(ERASE_SEQ); return true;
			case TERMUI_KEY_BKSPC:  erase_one(); break;
			}
		}
	}
}

void InputField::erase_one(void) {
	if (_position > 0) {
		_buffer.chr[--_position] = 0;
		printf("\033[1D\033[0K");
	}
}

bool InputField::get(long long * out) {
	bool captured = capture_input();
	//printf("\033[1Kcaptured: %s", _buffer.chr);
	if (captured) {
		try {
			*out = std::stoll(_buffer.chr);
		} catch (std::exception & e) {
			//printf("\033[1Kconversion error: %s", e.what());
			return false;
		}
		//printf("\033[1Kconverted: %lld", *out);
		return true;
	} else {
		//printf("\033[1Kinput canceled");
		return false;
	}
}
