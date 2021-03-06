#include "popup.h"
#include "term_ui.h"
#include <stdexcept>


#define LMARGIN "\033[%d;%dH"

int Popup::popup(int X, int Y, int W) const {
	int Y0 = Y;
	
	char * bar = new char[W+EX+EX+1];
	memset(bar, '-', W+EX+EX);
	bar[W+EX+EX] = 0;

	printf(LMARGIN "%*c"        , Y++, X, EX + 1 + EX + W + EX + 1 + EX, ' ');
	printf(LMARGIN "%*c+%-s+%*c", Y++, X, EX, ' ', bar, EX, ' ');
	printf(LMARGIN "%*c|%*c|%*c", Y++, X, EX, ' ', EX + W + EX, ' ', EX, ' ');

	for (auto & action : _to_exec) {
		printf(LMARGIN "%*c|%*c"    , Y++, X, EX, ' ', EX, ' ' );
		action();
		printf("%*c|%*c", EX, ' ', EX, ' ' );
	}

	printf(LMARGIN "%*c|%*c|%*c", Y++, X, EX, ' ', EX + W + EX, ' ', EX, ' ');
	printf(LMARGIN "%*c+%-s+%*c", Y++, X, EX, ' ', bar, EX, ' ');
	printf(LMARGIN "%*c"        , Y++, X, EX + 1 + EX + W + EX + 1 + EX, ' ');

	delete[] bar;
	return Y-Y0;
}

void Popup::clear(int X, int Y, int W, int H) const {
	for (int i = 0; i < H; ++i) {
		printf(LMARGIN "%*c", Y++, X, EX + 1 + EX + W + EX + 1 + EX, ' ');
	}
}


void Popup::show(int X, int Y, int W, bool wait) const {
	if (_term == nullptr) {
		throw std::logic_error("Popup TermUI not initialized");
	}

	int H = popup(X, Y, W);
	if (wait) _term->read();
	clear(X, Y, W, H);
}

Popup & Popup::build(std::function<void(void)> && action) {
	_to_exec.push_back(action);
	return *this;
}
