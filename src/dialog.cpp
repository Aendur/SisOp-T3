#include "dialog.h"
#include "layout.h"
#include "term_ui.h"

//#include <cstdio>
#include <stdexcept>

using std::string;
using std::vector;

Dialog::Dialog(TermUI * t, const DialogMessage & mssg, const DialogOptions & opts) : _term(t), _message(mssg), _options(opts) {
	if (_term == nullptr) {
		throw std::runtime_error("Term not initialized");
	} else if (mssg.size() < 1) {
		throw std::runtime_error("Dialog message needs at least 1 element");
	} else if (opts.size() < 1) {
		throw std::runtime_error("Dialog needs at least 1 option");
	}

	int msg_len = 0;
	for (const auto & msg : _message) { msg_len = (int) msg.length() > msg_len ? (int) msg.length() : msg_len; }
	
	int opt_len = 0;
	for (const auto & opt : _options) { opt_len += (int) opt.option.length(); }
	opt_len += EW * ((int)_options.size() - 1);

	_inner_width = msg_len > opt_len ? msg_len : opt_len;
	_outer_width = _inner_width + (EW + 1 + EW) * 2;
	_opts_width = opt_len;
	_height = (int) _message.size() + 1 + EH;

	_outer_line = new char[2 * _outer_width];
	_middl_line = new char[2 * _outer_width];
	_inner_line = new char[2 * _outer_width];


	char *h_bar = new char[EW + _inner_width + EW + 1];
	memset(h_bar, '-', EW + _inner_width + EW);
	h_bar[EW + _inner_width + EW] = 0;

	snprintf(_outer_line, 2 * _outer_width, "%*c", _outer_width, ' ');
	snprintf(_middl_line, 2 * _outer_width, "%*c+%s+%*c", EW, ' ', h_bar, EW, ' ');
	snprintf(_inner_line, 2 * _outer_width, "%*c|%*c|%*c", EW, ' ', EW + _inner_width + EW, ' ', EW, ' ');

	delete[] h_bar;
}

Dialog::~Dialog(void) {
	delete[] _outer_line;
	delete[] _middl_line;
	delete[] _inner_line;
}

int Dialog::query(int x0, int y0) {
	KeyCode key = TERMUI_KEY_UNDEFINED;
	select(0);
	show(x0, y0);
	
	int selection_confirmed = DIALOG_NO_SELECTION;
	while (selection_confirmed == DIALOG_NO_SELECTION && (key = _term->read()) != TERMUI_KEY_ESC) {
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

	return selection_confirmed == DIALOG_NO_SELECTION ? DIALOG_NO_SELECTION : _options[selection_confirmed].callback();
}

void Dialog::select(int opt) {
	if (0 <= opt && opt < _options.size() ) {
		_selection = opt;
	}
}

#define STR_POSITION "\033[%d;%dH"
#define STR_NEGATIVE "\033[7m"
#define STR_POSITIVE "\033[0m"
//#define STR_POSITIVE "\033[27m"

void Dialog::set_paddings(int * lpad, int * rpad, int width, int maxwidth) const {
	int remaining = maxwidth - width;
	*lpad = remaining / 2 + remaining % 2;
	*rpad = remaining / 2;
}


void Dialog::show(int X, int Y) const {
	printf(STR_POSITION "%s", Y++, X, _outer_line);
	printf(STR_POSITION "%s", Y++, X, _middl_line);
	printf(STR_POSITION "%s", Y++, X, _inner_line);

	int lpadding, rpadding;

	for (const string & line : _message) {
		set_paddings(&lpadding, &rpadding, (int)line.length(), _inner_width + EW + EW);
		printf(STR_POSITION "%*c|%*c%s%*c|%*c", Y++, X, EW, ' ', lpadding, ' ', line.c_str(), rpadding, ' ', EW, ' ');
	}

	printf(STR_POSITION "%s", Y++, X, _inner_line);

	set_paddings(&lpadding, &rpadding, _opts_width, _inner_width + EW + EW);
	printf(STR_POSITION "%*c|%*c", Y++, X, EW, ' ', lpadding, ' ');
	
	int i = 0;
	for (i = 0; i < _options.size()-1; ++i) {
		printf("%s%s" STR_POSITIVE "%*c", i == _selection ? STR_NEGATIVE : "", _options[i].option.c_str(), EW, ' ');
	}
	printf("%s%s" STR_POSITIVE "%*c|%*c", i == _selection ? STR_NEGATIVE : "", _options[i].option.c_str(), rpadding, ' ', EW, ' ');

	printf(STR_POSITION "%s", Y++, X, _inner_line);
	printf(STR_POSITION "%s", Y++, X, _middl_line);
	printf(STR_POSITION "%s", Y++, X, _outer_line);
}

void Dialog::clear(int X, int Y) const {
	for (int i = 1; i < _height - 1; ++i) {
		printf(STR_POSITION "%*c", Y+i, X, _outer_width, ' ');
	}
}
