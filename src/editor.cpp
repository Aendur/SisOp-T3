#include "editor.h"
#include "term_ui.h"
#include "device.h"
#include "page.h"
#include "utility.h"
#include "dialog.h"

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
		default: _edit_mode = EditMode::HEX;
	}
	switch(_edit_mode) {
		case EditMode::HEX: _input.set_maxlen(2) ; /*_input.set_endkey(TERMUI_KEY_SPACE) ;*/ break;
		case EditMode::CHR: _input.set_maxlen(1) ; /*_input.set_endkey(TERMUI_KEY_SPACE) ;*/ break;
		case EditMode::STR: _input.set_maxlen(64); /*_input.set_endkey(TERMUI_KEY_RETURN);*/ break;
		case EditMode::UNK:
		default: break;
	}
}

bool Editor::edit(Device & dev) {
	memcpy(_buffer[0], dev.buffer(0), _buf_len);
	memcpy(_buffer[1], dev.buffer(1), _buf_len);
	_page[0]->set(_buffer, dev.offset());
	_page[1]->set(_buffer, dev.offset());
	const int LEN = dev.geometry().BytesPerSector;
	_device_offset = dev.offset() - 2 * LEN;

	EditorAction action = edit_run();
	if (action == WRITE_CHANGES) {
		printf(LAYOUT_FREE "    CHANGES WRITTEN");
		write_changes(dev);
	} else {
		printf(LAYOUT_FREE "    CHANGES DISCARDED");
	}
	_history.clear();
	return action == WRITE_CHANGES;
}

void Editor::write_changes(Device & dev) {
	dev.seek(_device_offset, false);
	dev.write(_buffer);
	dev.seek(_device_offset, false);
}

Editor::EditorAction Editor::edit_run(void) {
	static const DialogOptions dialog_options = {
		{ "Keep editing"   , [](void) { return KEEP_EDITING; } },
		{ "Write & leave"  , [](void) { return WRITE_CHANGES; } },
		{ "Discard & leave", [](void) { return DISCARD_CHANGES; } },
	};
	static const int show_stack_size = 32;

	if (!_initialized) {
		printf(LAYOUT_FREE "input field not initialized");
		return DISCARD_CHANGES;
	}
	//printf(LAYOUT_FREE "INPUT: ");
	KeyCode key = TERMUI_KEY_UNDEFINED;
	if (_position < 0) {
		set_cursor(0);
	}
	
	_page[0]->toggle_edit(true);
	_page[1]->toggle_edit(true);
	
	_term->clear_screen();
	print_commands();
	print_stack(show_stack_size);
	_page[0]->print();
	_page[1]->print();

	unsigned char input_byte;
	char input_str[64];

	Dialog quit_dialog("Write changes to disk and leave editor?", dialog_options);
	EditorAction dialog_result = KEEP_EDITING;

	//while (((key = _term->read()) != TERMUI_KEY_ESC && key != TERMUI_KEY_INSERT) || (dialog_result = proc_dialog(quit_dialog)) == KEEP_EDITING) {
	while (((key = _term->read()) != TERMUI_KEY_ESC) || (dialog_result = proc_dialog(quit_dialog)) == KEEP_EDITING) {
		if (TERMUI_KEY_SPACE <= key && key <= TERMUI_KEY_TILDE) {
			switch(_edit_mode) {
				case EditMode::HEX: if(_input.get(&input_byte, key, true)) { push_byte(input_byte); }       ; break;
				case EditMode::CHR:                                          push_byte((unsigned char) key) ; break;
				case EditMode::STR: if(_input.get(input_str, 32, key, true)) { push_str(input_str); }      ; break;
				case EditMode::UNK: printf("UNK"); break;
				default:            printf("UNK"); break;
			}
		} else {
			switch (key) {
				case TERMUI_KEY_F1              : _page[0]->toggle_mode()  ; break;
				case TERMUI_KEY_F2              : _page[0]->toggle_view()  ; break;
				case TERMUI_KEY_F3              : _page[0]->switch_buff()  ; break;
				case TERMUI_KEY_F5              : _page[1]->toggle_mode()  ; break;
				case TERMUI_KEY_F6              : _page[1]->toggle_view()  ; break;
				case TERMUI_KEY_F7              : _page[1]->switch_buff()  ; break;
				case TERMUI_KEY_TAB             : switch_edit_mode()       ; break;
				case TERMUI_KEY_ARROW_UP        : move_cursor( -32, CursorMoveMode::WRAP)   ; break;
				case TERMUI_KEY_ARROW_DOWN      : move_cursor(  32, CursorMoveMode::WRAP)   ; break;
				case TERMUI_KEY_ARROW_LEFT      : move_cursor(  -1, CursorMoveMode::WRAP)   ; break;
				case TERMUI_KEY_ARROW_RIGHT     : move_cursor(   1, CursorMoveMode::WRAP)   ; break;
				case TERMUI_KEY_CTRL_ARROW_UP   : move_cursor(-256, CursorMoveMode::WRAP); break;
				case TERMUI_KEY_CTRL_ARROW_DOWN : move_cursor( 256, CursorMoveMode::WRAP); break;
				case TERMUI_KEY_CTRL_ARROW_RIGHT: move_cursor(   8, CursorMoveMode::WRAP); break;
				case TERMUI_KEY_CTRL_ARROW_LEFT : move_cursor(  -8, CursorMoveMode::WRAP); break;
				case TERMUI_KEY_HOME            : move_cursor(  -(_position % 32), CursorMoveMode::STAY); break;
				case TERMUI_KEY_END             : move_cursor(31-(_position % 32), CursorMoveMode::STAY); break;
				case TERMUI_KEY_BACKSPACE       : pop_byte(); break;
			}
		}

		print_commands();
		print_stack(show_stack_size);
		_page[0]->print();
		_page[1]->print();
	}

	_page[0]->toggle_edit(false);
	_page[1]->toggle_edit(false);
	_term->clear_screen();
	return dialog_result;
}

void Editor::print_commands(void) const {
	printf("\033[1;1H");
	printf("\n--- NAV ---                 \n");
	printf("ARROWS: move cursor by 1      \n");
	printf("C+ARR : move cursor by 8      \n");
	printf("HOME  : move to BOL           \n");
	printf("END   : move to EOL           \n");
	printf("\n--- DISP ---                \n");
	printf("INS   : stop editing\n");
	printf("TAB   : toggle edit mode    \n");
	printf("F1~3  : toggle disp 1 modes \n");
	printf("F5~7  : toggle disp 2 modes \n");
	printf("ESC   : stop editing        \n");
	printf("\nEDIT MODE: \033[1m");
	switch (_edit_mode) {
		case EditMode::HEX: printf("HEX"); break;
		case EditMode::CHR: printf("CHR"); break;
		case EditMode::STR: printf("STR"); break;
		case EditMode::UNK:
		default: printf("UNK"); break;
	}
	printf("\033[m");
}

void Editor::move_cursor(int offset, CursorMoveMode mode) {
	int future = _position + offset;
	int maxoff = 2 * (int) _buf_len;

	if (mode == CursorMoveMode::HALT) {
		if (future < 0) future = 0;
		else if (future >=  maxoff) { future = maxoff - 1; }
	} else if (mode == CursorMoveMode::WRAP) {
		if (future < 0) future = maxoff + (future % maxoff);
		else if (future >=  maxoff) future = future % maxoff;
	}
	
	set_cursor(future);
}

bool Editor::set_cursor(LONGLONG newpos) {
	LONGLONG maxoff = 2 * (int) _buf_len;
	if (0 <= newpos && newpos < maxoff) {
		_position = (int) newpos;
		_page[0]->select(_position);
		_page[1]->select(_position);
		printf(LAYOUT_FREE "          SELECTED: %d", _position);
		return true;
	} else {
		printf(LAYOUT_FREE "          SET_CURSOR FAILED");
		return false;
	}
}

void Editor::push_byte(unsigned char byte) {
	int buf_ind = _position / _buf_len;
	int buf_pos = _position % _buf_len;
	BYTE prev = _buffer[buf_ind][buf_pos];
	_history.emplace_front(_device_offset + _position, prev);
	_buffer[buf_ind][buf_pos] = byte;
	move_cursor(1, CursorMoveMode::STAY);
}

void Editor::push_str(const char * str) {
	int len = (int) strlen(str);
	int maxpos = _position + len;
	int maxoff = 2 * (int) _buf_len;
	int i, buf_ind, buf_pos;
	for (int newpos = _position; newpos < maxpos && newpos < maxoff; ++newpos) {
		i = newpos - _position;
		buf_ind = newpos / _buf_len;
		buf_pos = newpos % _buf_len;
		BYTE prev = _buffer[buf_ind][buf_pos];
		_history.emplace_front(_device_offset + newpos, prev);
		_buffer[buf_ind][buf_pos] = str[i];
	}
	move_cursor(len, CursorMoveMode::HALT);
}

void Editor::pop_byte(void) {
	if (!_history.empty()) {
		auto [pos, chr]= _history.front();
		LONGLONG rpos = pos - _device_offset;
		if (set_cursor(rpos)) {
			int buf_ind = _position / _buf_len;
			int buf_pos = _position % _buf_len;
			_buffer[buf_ind][buf_pos] = chr;
			_history.pop_front();
		} else {
			printf(LAYOUT_FREE "          UNABLE TO POP FROM STACK (OFFSET)");
		}
	} else {
		printf(LAYOUT_FREE "          STACK EMPTY     ");
	}
}

void Editor::print_stack(int max) {
	ColorizeOptions opts;
	opts.chr_hex = false;
	opts.ctl_str = "~";
	//printf("\033[0J\n\n--- MOD STACK ---\n");
	printf("\n--- MOD STACK ---\n");
	int i = 0;
	for(auto [pos, chr] : _history) {
		opts.byte = chr;
		printf("%3d. POS %08llx, BYTE %s (0x%02X)\n", ++i, pos, colorize_byte(opts), chr);
		if (i >= max) { break; }
	}
	if (_history.size() > max) {
		printf("%16s%16s\n", "...", "");
	}
	printf("%32s\n", "");
}

Editor::EditorAction Editor::proc_dialog(Dialog & dialog) {
	if (_history.empty()) {
		return DISCARD_CHANGES;
	}

	int code = dialog.query(80, 20);
	printf(LAYOUT_FREE "          DIALOG RETURNED %d", code);
	return (EditorAction)code;
}
