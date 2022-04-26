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

bool InputField::capture_input(KeyCode feed, bool reset) {
	if (_term == nullptr) {
		printf(LAYOUT_FREE "input field not initialized");
		return false;
	}

	if (reset) {
		_position = 0;
		_buffer.chr[0] = 0;
	}

	if (feed != _endkey && TERMUI_KEY_SPACE <= feed && feed <= TERMUI_KEY_TILDE) {
		_buffer.chr[_position++] = (char) feed;
		_buffer.chr[_position] = 0;
	}

	printf("\033[%d;%dH%s%s", _Y0, _X0, _message, _buffer.chr);
	KeyCode key = TERMUI_KEY_UNDEFINED;

	while (_position < _maxlen && (key = _term->read()) != _endkey && key != TERMUI_KEY_ESC) {

		if (TERMUI_KEY_SPACE <= key && key <= TERMUI_KEY_TILDE) {
			printf("%c\033[0K", key);
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

void InputField::print_error(const char * msg) {
	printf("     \033[31;1mError:\033[0m %s\033[0K", msg);
}

bool InputField::get(long long * out, KeyCode feed, bool keep_trying) {
	bool reset = true;
	do {
		bool captured = capture_input(feed);
		if (captured) {
			try {
				*out = std::stoll(_buffer.chr);
				return true;
			} catch (std::exception &) {
				feed = TERMUI_KEY_UNDEFINED;
				reset = false;
				//print_error(e.what());
				print_error("invalid decimal value");
			}
		} else {
			return false;
		}
	} while (keep_trying);
	return false;
}



#include <map>
static const std::map<char, int> hexmap = {
	{'0', 0x0}, {'1', 0x1}, {'2', 0x2}, {'3', 0x3},
	{'4', 0x4}, {'5', 0x5}, {'6', 0x6}, {'7', 0x7},
	{'8', 0x8}, {'9', 0x9}, {'A', 0xA}, {'B', 0xB},
	{'C', 0xC}, {'D', 0xD}, {'E', 0xE}, {'F', 0xF},
	{'a', 0xa}, {'b', 0xb}, {'c', 0xc}, {'d', 0xd},
	{'e', 0xe}, {'f', 0xf},
};

bool InputField::get(unsigned char * out, KeyCode feed, bool keep_trying) {
	do {
		bool captured = capture_input(feed);
		if (captured) {
			try {
				*out = (unsigned char)(0x00FF & ((hexmap.at(_buffer.chr[0]) << 4) | hexmap.at(_buffer.chr[1])));
				return true;
			} catch (std::exception &) {
				feed = TERMUI_KEY_UNDEFINED;
				//print_error(e.what());
				print_error("invalid hexadecimal value");
			}
		} else {
			return false;
		}
	} while (keep_trying);
	return false;
}

bool InputField::get(char * out, size_t max_len, KeyCode feed, bool keep_trying) {
	bool reset = true;
	do {
		bool captured = capture_input(feed, reset);
		if (captured) {
			try {
				size_t len = strlen(_buffer.chr);
				if (len > max_len) throw std::range_error("string too large: " + std::to_string(len) + " bytes (max " + std::to_string(max_len) + ")");
				memcpy(out, _buffer.chr, len);
				out[len] = 0;
				return true;
			} catch (std::exception & e) {
				feed = TERMUI_KEY_UNDEFINED;
				reset = false;
				print_error(e.what());
			}
		} else {
			return false;
		}
	} while (keep_trying);
	return false;
}
