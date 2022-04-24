#ifndef INPUT_FIELD_H
#define INPUT_FIELD_H

// #include <list>
// #include <string>

class TermUI;
class InputField {
private:
	TermUI * _term = nullptr;
	union {
		char     chr[512];
		wchar_t wchr[512];
	} _buffer;
	
	int _position = 0;
	//std::list<std::string> _history;
	void erase_one(void);
	bool capture_input(void);
public:
	inline void init(TermUI * t) { _term = t; }
	bool get(long long * out);
};


#endif
