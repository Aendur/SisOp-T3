#ifndef CLW_WIDGET_H
#define CLW_WIDGET_H

struct Coords {
	int x;
	int y;

	Coords operator+(const Coords & c2) const;
};

class Widget {
private:
	Widget * _parent = nullptr;
	Coords _pos; // = {0, 0};
	int _width;
	int _height;
public:
	Widget(Widget * parent, const Coords & pos, int w, int h);
	inline const Widget & get_parent(void) const { return *_parent; }
	inline const Coords & get_pos(void) const { return _pos; }
	inline const int get_width(void) const { return _width; }
	inline const int get_height(void) const { return _height; }
	virtual void print(void) const = 0;
};


#endif