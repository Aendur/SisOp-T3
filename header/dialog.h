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

typedef std::vector<std::string>  DialogMessage;
typedef std::vector<DialogOption> DialogOptions;

class TermUI;
class Dialog {
private:
	TermUI * _term = nullptr;
	const DialogMessage _message;
	const DialogOptions _options;

	static const int EW = 5;
	static const int EH = 7;

	//char ** _lines = nullptr;
	char * _outer_line = nullptr;
	char * _middl_line = nullptr;
	char * _inner_line = nullptr;
	int _inner_width;
	int _outer_width;
	int _opts_width;
	int _height;

	int _selection = 0;

	void select(int opt);
	void show(int x0, int y0) const;
	void clear(int x0, int y0) const;
	void set_paddings(int * lpad, int * rpad, int width, int maxwidth) const;
public:
	Dialog(TermUI* t, const DialogMessage & msg, const DialogOptions & opts);
	~Dialog(void);
	int query(int x0, int y0);
};


#endif
