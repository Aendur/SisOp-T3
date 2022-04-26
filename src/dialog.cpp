#include "dialog.h"
#include "layout.h"
#include "term_ui.h"

//#include <cstdio>
#include <stdexcept>

using std::string;
using std::vector;

Dialog::Dialog(TermUI * term, const string & msg, const vector<string> & opts) : _term(term), _msg(msg), _options(opts) {
	if (opts.size() < 1) {
		throw std::runtime_error("Dialog needs at least 1 option");
	}
	int msg_len = (int) msg.length();
	int opt_len = 0;
	for (const auto & opt : opts) { opt_len += (int) opt.length(); }

	int msg_width = 12 + msg_len;
	int opt_width = 3 * ((int)_options.size() + 1) + opt_len + 6;
	int extra_pad;

	if (msg_width > opt_width) {
		extra_pad = msg_width - opt_width;
		_line_width = msg_width;
		if (extra_pad % 2 == 1) {
			++_line_width;
			++_msg_rpad;
		}
		_opt_lpad = _opt_rpad = (extra_pad + 1) / 2;
	} else if (msg_width < opt_width)  {
		extra_pad = opt_width - msg_width;
		_line_width = opt_width;
		if (extra_pad % 2 == 1) {
			++_line_width;
			++_opt_rpad;
		}
		_msg_lpad = _msg_rpad = (extra_pad + 1) / 2;
	}


	for (int i = 0; i < _N_extra_lines; ++i) {
		_lines[i] = new char[_line_width + 1];
	}

	memset(_lines[0], '-', _line_width);
	memset(_lines[1], ' ', _line_width);

	_lines[0][0] = ' ';
	_lines[0][1] = ' ';
	_lines[0][2] = '+';
	_lines[0][_line_width] = 0;
	_lines[0][_line_width - 1] = ' ';
	_lines[0][_line_width - 2] = ' ';
	_lines[0][_line_width - 3] = '+';
	
	_lines[1][2] = '|';
	_lines[1][_line_width] = 0;
	_lines[1][_line_width - 3] = '|';
}

Dialog::~Dialog(void) {
	for (int i = 0; i < _N_extra_lines; ++i) {
		delete[] _lines[i];
	}
}

int Dialog::query(int x0, int y0) {
	KeyCode key = TERMUI_KEY_UNDEFINED;
	select(0);
	show(x0, y0);
	
	int selection_confirmed = -1;
	while (selection_confirmed == -1 && (key = _term->read()) != TERMUI_KEY_ESC) {
		switch (key) {
			case TERMUI_KEY_ARROW_UP   : select(_selection - 1); break;
			case TERMUI_KEY_ARROW_DOWN : select(_selection + 1); break;
			case TERMUI_KEY_ARROW_LEFT : select(_selection - 1); break;
			case TERMUI_KEY_ARROW_RIGHT: select(_selection + 1); break;
			case TERMUI_KEY_RETURN     : selection_confirmed = _selection; break;
		}
		show(x0, y0);
	}
	clear(x0, y0);
	return selection_confirmed;
}

void Dialog::select(int opt) {
	if (0 <= opt && opt < _options.size() ) {
		_selection = opt;
	}
}

#define STR_OUTER "\033[%d;%dH%*s"
#define STR_INNER "\033[%d;%dH%s"
#define STR_MESSG "\033[%d;%dH  |   %*s%s%*s   |  "
#define STR_OPTLP "\033[%d;%dH  |%*s"
#define STR_OPTRP "   %*s|  "
#define STR_OPTSY "   \033[7m%s\033[27m"
#define STR_OPTSN "   %s"

void Dialog::show(int x0, int y0) const {
	printf(STR_OUTER , y0 + 0, x0, _line_width, "");
	printf(STR_INNER , y0 + 1, x0, _lines[0]);
	printf(STR_INNER , y0 + 2, x0, _lines[1]);
	printf(STR_MESSG , y0 + 3, x0, _msg_lpad, "", _msg.c_str(), _msg_rpad, "");
	printf(STR_INNER , y0 + 4, x0, _lines[1]);
	
	printf(STR_OPTLP , y0 + 5, x0, _opt_lpad, "");
	for (int i = 0; i < _options.size(); ++i) {
		if (i == _selection) {
			printf(STR_OPTSY, _options[i].c_str());
		} else {
			printf(STR_OPTSN, _options[i].c_str());
		}
	}
	printf(STR_OPTRP , _opt_rpad, "");
	
	printf(STR_INNER , y0 + 6, x0, _lines[1]);
	printf(STR_INNER , y0 + 7, x0, _lines[0]);
	printf(STR_OUTER , y0 + 8, x0, _line_width, "");
}

void Dialog::clear(int x0, int y0) const {
	for (int y = 1; y < 8; ++y) {
		printf(STR_OUTER , y0 + y, x0 + 2, _line_width - 4, "");
	}
}
