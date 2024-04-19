#include <ncurses.h>
#include <string.h>
#include <panel.h>
#include <cstdlib>

typedef struct _PANEL_DATA {
	int x, y, w, h;
	char label[80];
	int label_color;
	PANEL *next;
} PANEL_DATA;

void set_user_ptrs(PANEL **panels, int n);

int main() {
	int ch;

	initscr();
	raw();
	int row, col;
	getmaxyx(stdscr, row, col);
	keypad(stdscr, true);
	noecho();
	start_color();
	init_pair(1, COLOR_CYAN, COLOR_BLACK);
	init_pair(2, COLOR_RED, COLOR_BLACK);
	init_pair(3, COLOR_GREEN, COLOR_BLACK);
	init_pair(4, COLOR_BLUE, COLOR_BLACK);
	
	if (has_colors() == false){
		endwin();
		printw("No colors\n");
		refresh();
		return 0;;
	}
	else{
		printw("Can show colors\n");
		refresh();
	}

	WINDOW* window[3];
	window[0] = newwin(10, 10, (row - 10) / 2, (col - 10) / 2);
	box(window[0], 0, 0);
	
	window[1] = newwin(12, 12, (row - 2) / 2, (col - 2) / 2);
	box(window[1], 0, 0);

	window[2] = newwin(15, 15, (row - 25) / 2, (col - 9) / 2);
	box(window[2], 0, 0);
	
//
	printw("Enter your character to see in reverse\n");
	ch = getch();
	char mesg[] = "You have pressed:\n";

	PANEL* panels[3];
	panels[0] = new_panel(window[0]);
	panels[1] = new_panel(window[1]);
	panels[2] = new_panel(window[2]);

	update_panels();
	doupdate();

	int newx, newy, neww, newh;
	PANEL* stack_top = panels[2];
	PANEL_DATA *top = (PANEL_DATA*) panel_userptr(stack_top);
	newx = top->x;
	newy = top->y;
	neww = top->w;
	newh = top->h;
	while((ch = getch()) != KEY_F(1)) {
		switch(ch) {
			case KEY_RIGHT:
				++newx;
				break;
			case KEY_UP:
				++newy;
				break;
			case KEY_LEFT:
				--newx;
				break;
			case KEY_DOWN:
				--newy;
				break;
			case 9:
				top = (PANEL_DATA*) panel_userptr(stack_top);
				top_panel(top->next);
				stack_top = top->next;
				top = (PANEL_DATA*) panel_userptr(stack_top);
				newx = top->x;
				newy = top->y;
				neww = top->w;
				newh = top->h;
				break;
			case 10:
				move(row - 4, 0);
				clrtoeol();
				refresh();
				move_panel(stack_top, newy, newx);
				break;
		}
		attron(COLOR_PAIR(4));
		mvprintw(row - 3, 0, "done");
		attroff(COLOR_PAIR(4));
		refresh();
		update_panels();
		doupdate();
	}

//	mvprintw(row/2, (col - strlen(mesg))/2, "%s", mesg);
//	mvaddch(row/2 + 1, (col - strlen(mesg))/2, ch);
//	mvchgat(row/2 + 1, (col - strlen(mesg))/2, -1, A_BLINK, 1, NULL);
//
//	refresh();
//	delwin(window);
	getch();
	endwin();
// asdfaasdf
	return 0;
}

void set_user_ptrs(PANEL **panels, int n) {
	PANEL_DATA *ptrs;
	WINDOW *win;
	int x, y, w, h;
	char temp[80];

	ptrs = (PANEL_DATA*)calloc(n, sizeof(PANEL_DATA));
	for (int i = 0;i < n; ++i) {
		win = panel_window(panels[i]);
		getbegyx(win, y, x);
		getmaxyx(win, h, w);
		ptrs[i].x = x;
		ptrs[i].y = y;
		ptrs[i].w = w;
		ptrs[i].h = h;
		sprintf(temp, "Window Number %d", i + 1);
		strcpy(ptrs[i].label, temp);
		ptrs[i].label_color = i + 1;
		if (i + 1 == n)
			ptrs[i].next = panels[0];
		else
			ptrs[i].next = panels[i + 1];
		set_panel_userptr(panels[i], &ptrs[i]);
	}
}
