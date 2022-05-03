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

		_buf_len = nbytes;
		_buffer[0] = new BYTE[_buf_len];
		_buffer[1] = new BYTE[_buf_len];
		_position = -1;
		_initialized = true;
	}
}

void Editor::switch_edit_mode(void) {
	switch(_edit_mode) {
		case EditMode::CHR: _edit_mode = EditMode::HEX; break;
		case EditMode::HEX: _edit_mode = EditMode::FIL; break;
		case EditMode::FIL: _edit_mode = EditMode::CHR; break;
		default: _edit_mode = EditMode::CHR;
	}
	setup_edit_mode();
}

void Editor::setup_edit_mode(void) {
	switch(_edit_mode) {
		case EditMode::HEX: _input.set_maxlen(2) ; _input.set_msg("INPUT BYTE: "); /*_input.set_endkey(TERMUI_KEY_SPACE) ;*/ break;
		case EditMode::CHR: _input.set_maxlen(1) ; _input.set_msg("INPUT CHAR: "); /*_input.set_endkey(TERMUI_KEY_SPACE) ;*/ break;
		case EditMode::FIL: _input.set_maxlen(2) ; _input.set_msg("INPUT BYTE: "); /*_input.set_endkey(TERMUI_KEY_RETURN);*/ break;
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
		dev.seek(_device_offset, false);
		try {
			dev.write(_buffer);
		} catch (std::exception &) {
			//printf(LAYOUT_FREE "     ERROR WRITING TO DISK: %s\033[0K", e.what());
		}
	}
	dev.seek(_device_offset, false);
	_history.clear();
	return action == WRITE_CHANGES;
}

Editor::EditorAction Editor::edit_run(void) {
	static const DialogOptions dialog_options = {
		{ "Keep editing"   , [](void) { return KEEP_EDITING; } },
		{ "Write & leave"  , [](void) { return WRITE_CHANGES; } },
		{ "Discard & leave", [](void) { return DISCARD_CHANGES; } },
	};
	static const int show_stack_size = 42;

	if (!_initialized) {
		printf(LAYOUT_FREE "input field not initialized");
		return DISCARD_CHANGES;
	}
	//printf(LAYOUT_FREE "INPUT: ");
	KeyCode key = TERMUI_KEY_UNDEFINED;
	if (_position < 0) {
		select(0);
	}
	
	_page[0]->toggle_edit(true);
	_page[1]->toggle_edit(true);
	
	_term->clear_screen();
	_page[0]->print();
	_page[1]->print();
	print_commands();
	print_stack(show_stack_size);

	Dialog quit_dialog(_term, {"Write changes to disk and leave editor?"}, dialog_options);
	EditorAction dialog_result = KEEP_EDITING;

	unsigned char input_byte;
	//char input_char;
	while (((key = _term->read()) != TERMUI_KEY_ESC) || (dialog_result = proc_dialog(quit_dialog)) == KEEP_EDITING) {
		if (TERMUI_KEY_SPACE <= key && key <= TERMUI_KEY_TILDE) {
			switch(_edit_mode) {
				case EditMode::HEX: if(_input.get(&input_byte, key, true)) { push_byte(input_byte); }       ; break;
				case EditMode::CHR:                                          push_byte((unsigned char) key) ; break;
				case EditMode::FIL: if(_input.get(&input_byte, key, true)) { push_fill(input_byte); }       ; break;
				default:            printf("UNK"); break;
			}
		} else if (key == TERMUI_KEY_RETURN && _edit_mode == EditMode::CHR) {
			push_byte((unsigned char) key);
		} else {
			switch (key) {
				case TERMUI_KEY_F1         : _page[0]->cycle_sectors_views()                   ; break;
				case TERMUI_KEY_F2         : _page[0]->cycle_entries_views()                   ; break;
				case TERMUI_KEY_F3         : _page[0]->switch_text()                           ; break;
				case TERMUI_KEY_F4         : _page[0]->switch_buff()                           ; break;
				case TERMUI_KEY_F5         : _page[1]->cycle_sectors_views()                   ; break;
				case TERMUI_KEY_F6         : _page[1]->cycle_entries_views()                   ; break;
				case TERMUI_KEY_F7         : _page[1]->switch_text()                           ; break;
				case TERMUI_KEY_F8         : _page[1]->switch_buff()                           ; break;
				case TERMUI_KEY_TAB             : switch_edit_mode()                           ; break;
				case TERMUI_KEY_SHIFT_TAB       : convert_value()                              ; break;
				case TERMUI_KEY_ARROW_UP        : move( -32, CursorMoveMode::WRAP); break;
				case TERMUI_KEY_ARROW_DOWN      : move(  32, CursorMoveMode::WRAP); break;
				case TERMUI_KEY_ARROW_LEFT      : move(  -1, CursorMoveMode::WRAP); break;
				case TERMUI_KEY_ARROW_RIGHT     : move(   1, CursorMoveMode::WRAP); break;
				case TERMUI_KEY_CTRL_ARROW_UP   : move(-256, CursorMoveMode::WRAP); break;
				case TERMUI_KEY_CTRL_ARROW_DOWN : move( 256, CursorMoveMode::WRAP); break;
				case TERMUI_KEY_CTRL_ARROW_RIGHT: move(   8, CursorMoveMode::WRAP); break;
				case TERMUI_KEY_CTRL_ARROW_LEFT : move(  -8, CursorMoveMode::WRAP); break;
				case TERMUI_KEY_HOME            : move(  -(_position % 32), CursorMoveMode::STAY); break;
				case TERMUI_KEY_END             : move(31-(_position % 32), CursorMoveMode::STAY); break;
				case TERMUI_KEY_BACKSPACE       : pop_byte(); break;
			}
		}

		_page[0]->print();
		_page[1]->print();
		print_commands();
		print_stack(show_stack_size);
	}

	_page[0]->toggle_edit(false);
	_page[1]->toggle_edit(false);
	_term->clear_screen();
	return dialog_result;
}

void Editor::print_commands(void) const {
	_term->clear_column(1,1,32,47);
	printf("\n--- NAV ---                 \n");
	printf("ARROWS: move cursor by 1      \n");
	printf("C+ARR : move cursor by 8      \n");
	printf("HOME  : move to BOL           \n");
	printf("END   : move to EOL           \n");
	printf("\n--- DISP ---                \n");
	printf("TAB   : toggle edit mode    \n");
	printf("F1    : toggle disp 1 modes \n");
	printf("F2    : toggle disp 2 modes \n");
	printf("ESC   : stop editing        \n");
	
	ColorizeOptions opts;
	opts.chr_hex = false;
	opts.ctl_str = "~";
	opts.byte = (unsigned char)converted_value;

	printf("\nSHIFT+TAB: convert value\n");
	printf("HEX: \033[1m%X\033[0m  \n", (unsigned) converted_value);
	printf("DEC: \033[1m%lld\033[0m  \n", converted_value);
	printf("OCT: \033[1m%o\033[0m  \n", (unsigned) converted_value);
	printf("CHR: %s     \n", colorize_byte(opts));

	printf("\nEDIT MODE: \033[1m");
	switch (_edit_mode) {
		case EditMode::HEX: printf("HEX "); break;
		case EditMode::CHR: printf("CHR "); break;
		case EditMode::FIL: printf("FILL"); break;
		default: printf("UNK"); break;
	}
	printf("\033[m");
}

void Editor::move(int offset, CursorMoveMode mode) {
	int future = _position + offset;
	int maxoff = 2 * (int) _buf_len;

	if (mode == CursorMoveMode::HALT) {
		if (future < 0) future = 0;
		else if (future >=  maxoff) { future = maxoff - 1; }
	} else if (mode == CursorMoveMode::WRAP) {
		if (future < 0) future = maxoff + (future % maxoff);
		else if (future >=  maxoff) future = future % maxoff;
	}
	
	select(future);
}

bool Editor::select(LONGLONG newpos) {
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
	move(1, CursorMoveMode::STAY);
}

void Editor::push_fill(unsigned char byte) {
	ColorizeOptions opts;
	opts.chr_hex = false;
	opts.ctl_str = "~";
	opts.byte = byte;
	
	int len;
	char msg[64];
	snprintf(msg, 64, "FILL 0x%02X (%s) - BYTE COUNT: ", byte, colorize_byte(opts));
	
	_input.set_maxlen(12);
	_input.set_msg(msg);
	
	if (_input.get(&len, TERMUI_KEY_UNDEFINED, true)) {
		int maxpos = _position + len;
		int maxoff = 2 * (int) _buf_len;
		int buf_ind, buf_pos;
		for (int newpos = _position; newpos < maxpos && newpos < maxoff; ++newpos) {
			buf_ind = newpos / _buf_len;
			buf_pos = newpos % _buf_len;
			BYTE prev = _buffer[buf_ind][buf_pos];
			_history.emplace_front(_device_offset + newpos, prev);
			_buffer[buf_ind][buf_pos] = byte;
		}
		move(len, CursorMoveMode::HALT);
	}
	setup_edit_mode();
}

void Editor::pop_byte(void) {
	if (!_history.empty()) {
		auto [pos, chr]= _history.front();
		LONGLONG rpos = pos - _device_offset;
		if (select(rpos)) {
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
	//_term->clear_column(204,1,32,47);
	ColorizeOptions opts;
	opts.chr_hex = false;
	opts.ctl_str = "~";

	printf("\033[2;205H--- MOD STACK ---\n");
	int i = 0;
	for(auto [pos, chr] : _history) {
		opts.byte = chr;
		//printf("\033[204C%2d: POS %08llx BYTE %s (%02X)\n", ++i, pos, colorize_byte(opts), chr);
		//if (i >= max) { break; }
		
		printf("\033[204C%d: POS %08llx BYTE %s (%02X) \n", (int)_history.size() - i, pos, colorize_byte(opts), chr);
		if (++i >= max) { break; }
	}
	int remaining = (int)_history.size() - max;
	if (remaining > 0) {
		printf("\033[204C%12s%-3d%17s\n", "+", remaining, "");
	}
	printf("\033[204C%32s\n", "");
}

Editor::EditorAction Editor::proc_dialog(Dialog & dialog) {
	if (_history.empty()) {
		return DISCARD_CHANGES;
	}

	int code = dialog.query(80, 20);
	printf(LAYOUT_FREE "          DIALOG RETURNED %d", code);
	return (EditorAction)code;
}

void Editor::convert_value(void) {
	_input.set_maxlen(20);
	_input.set_msg("\033[1mCONVERT VALUE:\033[0m ");
	_input.get(&converted_value, TERMUI_KEY_UNDEFINED, true);
	setup_edit_mode();
}
