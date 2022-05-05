#ifndef POPUP_H
#define POPUP_H

#include <vector>
#include <functional>

class TermUI;
class Popup {
private:
	static const int EX = 3;
	TermUI * _term = nullptr;
	std::vector<std::function<void(void)>> _to_exec;
	int popup(int X, int Y, int W) const;
	void clear(int X, int Y, int W, int H) const;
public:
	~Popup(void) {}
	Popup(TermUI * t) : _term(t) { }
	Popup& build(std::function<void(void)> && action);
	void show(int X, int Y, int W, bool wait = true) const;
};

#endif
