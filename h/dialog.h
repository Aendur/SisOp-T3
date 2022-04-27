#ifndef DIALOG_H
#define DIALOG_H

#include <vector>
#include <string>
#include <functional>

typedef std::function<int(void)> DialogCallback;
struct DialogOption {
	const std::string option;
	const DialogCallback callback;
};

typedef std::vector<DialogOption> DialogOptions;

enum DialogSelection {
	DIALOG_NO_SELECTION = -1,
};

class TermUI;
class Dialog {
private:
	inline static TermUI * _term = nullptr;
	inline static bool _initialized = false;

	const std::string _msg;
	const DialogOptions _options;

	static const int _N_extra_lines = 2;
	char * _lines[_N_extra_lines];
	int _line_width;
	int _msg_lpad = 0;
	int _msg_rpad = 0;
	int _opt_lpad = 0;
	int _opt_rpad = 0;

	
	int _selection = 0;

	void select(int opt);
	void show(int x0, int y0) const;
	void clear(int x0, int y0) const;
public:
	inline static void init(TermUI * t) { if(!_initialized) {_term = t; _initialized = true;} }

	Dialog(const std::string & msg, const DialogOptions & opts);
	~Dialog(void);
	int query(int x0, int y0);
};


#endif
