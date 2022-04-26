#include "editor.h"
#include "layout.h"
#include "term_ui.h"
#include "device.h"
#include "page.h"

#include <cstdio>
#include <stdexcept>

// #define ERASE_SEQ "\033[1K"

Editor::~Editor(void) {
	delete[] _buffer[0];
	delete[] _buffer[1];
}

void Editor::init(DWORD nbytes, TermUI * t, Page * p1, Page * p2) {
	if (!_initialized) {
		_term = t;
		_page[0] = p1;
		_page[1] = p2;
		_input.init(t, 48, 20, "EDITING: ");
		_buf_len = nbytes;

		_buffer[0] = new BYTE[_buf_len];
		_buffer[1] = new BYTE[_buf_len];
		_position = -1;
		_initialized = true;
	}
}

bool Editor::edit(const Device & dev) {
	memcpy(_buffer[0], dev.buffer(0), _buf_len);
	memcpy(_buffer[1], dev.buffer(1), _buf_len);
	_page[0]->set(_buffer, dev.offset());
	_page[1]->set(_buffer, dev.offset());

	return edit_run();
}


bool Editor::edit_run(void) {
	if (!_initialized) {
		printf(LAYOUT_FREE "input field not initialized");
		return false;
	}
	//printf(LAYOUT_FREE "INPUT: ");
	KeyCode key = TERMUI_KEY_UNDEFINED;
	if (_position < 0) {
		_position = 0;
		_page[0]->select(_position);
		_page[1]->select(_position);
	}
	
	_page[0]->toggle_edit(true);
	_page[1]->toggle_edit(true);
	
	//printf("\033[48;10H%d", _page[0]->selected());
	//printf("\033[49;10H%d", _page[1]->selected());

	_page[0]->print();
	_page[1]->print();

	while ((key = _term->read()) != TERMUI_KEY_ESC) {
		switch (key) {
			case TERMUI_KEY_ARROW_UP:    move_cursor(-0x20); break;
			case TERMUI_KEY_ARROW_DOWN:  move_cursor(0x20); break;
			case TERMUI_KEY_ARROW_LEFT:  move_cursor(-1); break;
			case TERMUI_KEY_ARROW_RIGHT: move_cursor(1); break;
		}

		_page[0]->print();
		_page[1]->print();
	}

	_page[0]->toggle_edit(false);
	_page[1]->toggle_edit(false);
	_page[0]->print();
	_page[1]->print();
}


void Editor::move_cursor(int offset) {
	int future = _position + offset;
	if (0 <= future && future < 2 * (int) _buf_len) {
		_position = future;
		_page[0]->select(_position);
		_page[1]->select(_position);
	}
	printf(LAYOUT_FREE "          SELECTED: %d", _position);
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
