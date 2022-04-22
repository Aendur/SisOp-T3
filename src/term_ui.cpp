#include "term_ui.h"

#include <stdexcept>

TermUI::TermUI(void) {
	static const auto x = ~ENABLE_LINE_INPUT;
	_mode1_in = ENABLE_VIRTUAL_TERMINAL_INPUT;
	_mode1_out = ENABLE_VIRTUAL_TERMINAL_PROCESSING | DISABLE_NEWLINE_AUTO_RETURN | ENABLE_PROCESSED_OUTPUT;
	_mode1_err = ENABLE_VIRTUAL_TERMINAL_PROCESSING | DISABLE_NEWLINE_AUTO_RETURN | ENABLE_PROCESSED_OUTPUT;
	_mode2_in = (DWORD) ~ENABLE_LINE_INPUT & ~ENABLE_ECHO_INPUT;
}

TermUI::~TermUI(void) {
	if (
		_handle_in != NULL &&
		_handle_in != INVALID_HANDLE_VALUE &&
		_mode0_in != -1
	) { SetConsoleMode(_handle_in, _mode0_in); }
	if (
		_handle_out != NULL &&
		_handle_out != INVALID_HANDLE_VALUE &&
		_mode0_out != -1
	) { SetConsoleMode(_handle_out, _mode0_out); }
	if (
		_handle_err != NULL &&
		_handle_err != INVALID_HANDLE_VALUE &&
		_mode0_err != -1
	) { SetConsoleMode(_handle_err, _mode0_err); }
}

void TermUI::init(void) {
	init_in();
	init_out();
}

const TCHAR * TermUI::read(void) {
	BOOL status = ReadConsole(_handle_in, _input_string, BUFSIZE, &_input_nreads, NULL);
	if (status) { handle_input(); }
	return _input_string;
}

void TermUI::init_in(void) {
	_handle_in = GetStdHandle(STD_INPUT_HANDLE);
	if (_handle_in == INVALID_HANDLE_VALUE) {
		throw std::runtime_error("failed to get handle to stdin");
	}
	
	if (!GetConsoleMode(_handle_in, &_mode0_in)) {
		throw std::runtime_error("failed to save stdin state");
	}

	if (!SetConsoleMode(_handle_in, (_mode0_in | _mode1_in) & _mode2_in)) {
		throw std::runtime_error("failed to set new stdin state");
	}
}

void TermUI::init_out(void) {
	_handle_out = GetStdHandle(STD_OUTPUT_HANDLE);
	if (_handle_out == INVALID_HANDLE_VALUE) {
		throw std::runtime_error("failed to get handle to stdout");
	}
	
	if (!GetConsoleMode(_handle_out, &_mode0_out)) {
		throw std::runtime_error("failed to save stdout state");
	}

	if (!SetConsoleMode(_handle_out, (_mode0_out | _mode1_out) & _mode2_out)) {
		throw std::runtime_error("failed to set new stdout state");
	}
}

void TermUI::init_err(void) {
	_handle_err = GetStdHandle(STD_ERROR_HANDLE);
	if (_handle_err == INVALID_HANDLE_VALUE) {
		throw std::runtime_error("failed to get handle to stderr");
	}
	
	if (!GetConsoleMode(_handle_err, &_mode0_err)) {
		throw std::runtime_error("failed to save stderr state");
	}

	if (!SetConsoleMode(_handle_err, (_mode0_err | _mode1_err) & _mode2_err)) {
		throw std::runtime_error("failed to set new stderr state");
	}
}

void TermUI::handle_input(void) {
	_input_string[_input_nreads] = 0;
	
	if (_input_nreads == 0) { return; }

	switch(_input_string[0]) {
		case 033:
			//WriteConsole(_handle_out, esc, 15 * sizeof(TCHAR), NULL, NULL);
			//WriteConsole(_handle_out, &_input_string[1], size-1, NULL, NULL);
			wprintf(L"\033[31;1mESC\033[0m%hs", &_input_string[1]);
			break;
		case '\n':
			wprintf(L"[LF]");
			break;
		case '\r':
			wprintf(L"[CR]");
			break;
		default:
			wprintf(L"%hs", _input_string);
			break;
	}

	_input_nreads = 0;
}
