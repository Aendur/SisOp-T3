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
		_input.init(t, 38, 24, "EDITING: ");
		switch_edit_mode();


		_buf_len = nbytes;
		_buffer[0] = new BYTE[_buf_len];
		_buffer[1] = new BYTE[_buf_len];
		_position = -1;
		_initialized = true;
	}
}

void Editor::switch_edit_mode(void) {
	switch(_edit_mode) {
		case EditMode::HEX: _edit_mode = EditMode::CHR; break;
		case EditMode::CHR: _edit_mode = EditMode::STR; break;
		case EditMode::STR: _edit_mode = EditMode::HEX; break;
		case EditMode::UNK:
		default: _edit_mode = EditMode::CHR;
	}
	switch(_edit_mode) {
		case EditMode::HEX: _input.set_maxlen(2) ; /*_input.set_endkey(TERMUI_KEY_SPACE) ;*/ break;
		case EditMode::CHR: _input.set_maxlen(1) ; /*_input.set_endkey(TERMUI_KEY_SPACE) ;*/ break;
		case EditMode::STR: _input.set_maxlen(64); /*_input.set_endkey(TERMUI_KEY_RETURN);*/ break;
		case EditMode::UNK:
		default: break;
	}
}

bool Editor::edit(const Device & dev) {
	memcpy(_buffer[0], dev.buffer(0), _buf_len);
	memcpy(_buffer[1], dev.buffer(1), _buf_len);
	_page[0]->set(_buffer, dev.offset());
	_page[1]->set(_buffer, dev.offset());

	return edit_run();
}


void Editor::print_commands(void) const {
	printf("\033[1;1H");
	printf("\n-- NAV --                 \n");
	printf("ARROWS: move cursor         \n");
	printf("\n-- DISP --                \n");
	printf("INS   : stop editing\n");
	printf("TAB   : toggle edit mode    \n");
	printf("F1~3  : toggle disp 1 modes \n");
	printf("F5~7  : toggle disp 2 modes \n");
	printf("ESC   : stop editing        \n");
	printf("\n\n\nEDIT MODE: \033[1m");
	switch (_edit_mode) {
		case EditMode::HEX: printf("HEX"); break;
		case EditMode::CHR: printf("CHR"); break;
		case EditMode::STR: printf("STR"); break;
		case EditMode::UNK:
		default: printf("UNK"); break;
	}
	printf("\033[m");
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

	_term->clear_screen();
	print_commands();
	_page[0]->print();
	_page[1]->print();

	unsigned char input_byte;
	char input_str[64];
	while ((key = _term->read()) != TERMUI_KEY_ESC && key != TERMUI_KEY_INSERT) {
		if (TERMUI_KEY_SPACE <= key && key <= TERMUI_KEY_TILDE) {
			switch(_edit_mode) {
				case EditMode::HEX:
					_input.get(&input_byte, key, true);
					break;
				case EditMode::CHR:
					//_input.get(&input_byte, key, false);
					input_byte = (unsigned char) key;
					break;
				case EditMode::STR:
					_input.get(input_str, 32, key, true);
					break;
				case EditMode::UNK:
				default: printf("UNK"); break;
			}
			printf(LAYOUT_FREE "CAPTURED: %c %x %s", input_byte, input_byte, input_str);
		} else {
			switch (key) {
				case TERMUI_KEY_F1         : _page[0]->toggle_mode()  ; break;
				case TERMUI_KEY_F2         : _page[0]->toggle_view()  ; break;
				case TERMUI_KEY_F3         : _page[0]->switch_buff()  ; break;
				case TERMUI_KEY_F5         : _page[1]->toggle_mode()  ; break;
				case TERMUI_KEY_F6         : _page[1]->toggle_view()  ; break;
				case TERMUI_KEY_F7         : _page[1]->switch_buff()  ; break;
				case TERMUI_KEY_TAB        : switch_edit_mode()       ; break;
				case TERMUI_KEY_ARROW_UP   : move_cursor(-0x20)       ; break;
				case TERMUI_KEY_ARROW_DOWN : move_cursor(0x20)        ; break;
				case TERMUI_KEY_ARROW_LEFT : move_cursor(-1)          ; break;
				case TERMUI_KEY_ARROW_RIGHT: move_cursor(1)           ; break;
			}
		}

		print_commands();
		_page[0]->print();
		_page[1]->print();
	}

	_page[0]->toggle_edit(false);
	_page[1]->toggle_edit(false);
	return false;
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
