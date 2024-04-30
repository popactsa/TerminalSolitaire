#include "Element.h"

Element::Element(): win{nullptr}, pan{nullptr} {
	
}

WINDOW* Element::get_win() {
	if (win == nullptr)
		create_win();
	return win;
}

void Element::del_pan(bool _to_refresh) {
	del_panel(pan);
	pan = nullptr;
	if (_to_refresh == true) {
		update_panels();
		doupdate();
	}
}

PANEL* Element::get_pan() {
	if (pan == nullptr) {
		if (win == nullptr)
			create_win();
		pan = new_panel(win);
	}
	return pan;
}

void Element::set_win_parameters(int _height, int _width, int _y0, int _x0, int _color_scheme, chtype _ls, chtype _rs, chtype _ts, chtype _bs, chtype _tl, chtype _tr, chtype _bl, chtype _br) {
	ls = _ls;
	rs = _rs;
	ts = _ts;
	bs = _bs;
	tl = _tl;
	tr = _tr;
	bl = _bl;
	br = _br;


	height = _height;
	width = _width;
	y0 = _y0;
	x0 = _x0;

	color_scheme = _color_scheme;
}

void Element::create_win(bool _to_refresh) {
	win = newwin(height, width, y0, x0);
	wborder(win, ls, rs, ts, bs, tl, tr, bl, br);
	wbkgd(win, COLOR_PAIR(color_scheme));
	_to_refresh == true ? wrefresh(win) : true ;
}

void Element::set_coords(int _y0, int _x0) {
	y0 = _y0;
	x0 = _x0;
}

void Element::update_pan(bool _top_or_bottom, bool _to_refresh) {
	del_panel(pan);
	create_win();
	pan = new_panel(win);
	if (_top_or_bottom == true)
		top_panel(pan);
	else
		bottom_panel(pan);
	if (_to_refresh == true) {
		update_panels();
		doupdate();
	}
}

void Element::move_pan(bool _to_refresh) {
	move_panel(pan, y0, x0);
	if (_to_refresh) {
		update_panels();
		doupdate();
	}	
}

//Element::~Element() {
//	del_panel(pan);
//	delwin(win);
//}
