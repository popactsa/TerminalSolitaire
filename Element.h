#ifndef ELEMENT_H
#define ELEMENT_H

#include <ncurses.h>
#include <panel.h>
#include <string>

class Element{
	public:
		chtype ls, rs, ts, bs,
		       tl, tr, bl, br;
		int y0, x0;
		int height, width;
		int color_scheme;
		WINDOW* win;
		PANEL* pan;


		Element();
		WINDOW* get_win();
		PANEL* get_pan();
		void set_win_parameters(int _height = 3, int _width = 3, int _y0 = 0, int _x0 = 0, int _color_scheme = 1, chtype _ls = ACS_VLINE, chtype _rs = ACS_VLINE, chtype _ts = ACS_HLINE, chtype _bs = ACS_HLINE, chtype _tl = ACS_ULCORNER, chtype _tr = ACS_URCORNER, chtype _bl = ACS_LLCORNER, chtype _br = ACS_LRCORNER);;
		void create_win(bool _to_refresh = false);
		void del_pan(bool _to_refresh = false);
		void update_pan(bool _top_or_bottom = true, bool _to_refresh = true);
		void move_pan(bool _to_refresh = true);
		void set_coords(int _y0, int _x0);
		void write_in_middle(std::string _mesg);
		void write_item_value(const int _value);
		void write_item_value(const bool _value);
		void set_color_scheme(int i);
		~Element();
};

#endif
