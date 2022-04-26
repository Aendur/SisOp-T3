#ifndef DIALOG_H
#define DIALOG_H

#include <vector>
#include <string>

class TermUI;
class Dialog {
private:
	TermUI * _term;
	const std::string _msg;
	const std::vector<std::string> _options;

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
	Dialog(TermUI * term, const std::string & msg, const std::vector<std::string> & opts);
	~Dialog(void);
	int query(int x0, int y0);
};


#endif
