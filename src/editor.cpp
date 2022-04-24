#include "editor.h"
#include "layout.h"
#include "term_ui.h"

#include <cstdio>
#include <stdexcept>

#define ERASE_SEQ "\033[1K"

void Editor::init(TermUI * t, Page * p) {
	_term = t;
	_page = p;
	_input.init(t);
	_initialized = true;
}

bool Editor::edit(void) {
	if (!_initialized) {
		printf(LAYOUT_FREE "input field not initialized");
		return false;
	}
	//_position = 0;

	printf(LAYOUT_FREE "INPUT: ");
	KeyCode key = TERMUI_KEY_UNDEFINED;

	while (true) {
		key = _term->read();

		//if (TERMUI_KEY_SPACE <= key && key <= TERMUI_KEY_TILDE) {
		//	printf("%c", key);
		//	_buffer.chr[_position++] = (char) key;
		//	_buffer.chr[_position] = 0;
		//}

		switch (key) {
			case TERMUI_KEY_ESC: return false;
			//case TERMUI_KEY_RETURN: printf(ERASE_SEQ); return true;
			//case TERMUI_KEY_BKSPC:  erase_one(); break;
			case TERMUI_KEY_ARROW_UP:    move_cursor(-0x20); break;
			case TERMUI_KEY_ARROW_DOWN:  move_cursor(0x20); break;
			case TERMUI_KEY_ARROW_LEFT:  move_cursor(-1); break;
			case TERMUI_KEY_ARROW_RIGHT: move_cursor(1); break;
		}
	}
}


void Editor::move_cursor(int offset) {
	int future = _position + offset;
	if (0 <= future && future < 512) {
		_position = future;
	}
}

// void Editor::erase_one(void) {
// 	if (_position > 0) {
// 		_buffer.chr[--_position] = 0;
// 		printf("\033[1D\033[0K");
// 	}
// }

// bool Editor::get(long long * out) {
// 	bool captured = capture_input();
// 	if (captured) {
// 		try {
// 			*out = std::stoll(_buffer.chr);
// 		} catch (std::exception & e) {
// 			printf("\033[1KError: %s", e.what());
// 			return false;
// 		}
// 		return true;
// 	} else {
// 		return false;
// 	}
// }
