#include "clw/widget.h"

Coords Coords::operator+(const Coords & c2) const {
	return {this->x + c2.x, this->y + c2.y};
}

Widget::Widget(Widget * parent, const Coords & pos, int w, int h) : _parent(parent), _width(w), _height(h) {
	if (_parent == nullptr) {
		_pos = pos;
	} else {
		_pos = _parent->_pos + pos;
	}
}
