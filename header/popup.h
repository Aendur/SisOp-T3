#ifndef POPUP_H
#define POPUP_H

#include <vector>
#include <functional>

class TermUI;
class Popup {
private:
	TermUI * _term = nullptr;
	std::vector<std::function<void(void)>> _to_exec;
	void popup(int X, int Y, int W) const;
public:
	~Popup(void) {}
	Popup(TermUI * t) : _term(t) { }
	Popup& build(std::function<void(void)> && action);
	void show(int X, int Y, int W) const;
};

#endif
