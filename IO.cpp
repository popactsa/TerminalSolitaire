#include "IO.h"

#define debug printw("debug"); refresh();

IO::IO(int _n_stacks, int _n_out, int _n_deck_shown, int _noc, int _noe): n_stacks{_n_stacks}, n_out{_n_out}, n_deck_shown{_n_deck_shown}, noc{_noc}, noe{_noe} {
	setlocale(LC_ALL, "");
	std::srand(std::time(nullptr));

	initscr();
	cbreak();
	noecho();
	start_color();
	raw();
	refresh();

	init_pair(1, COLOR_GREEN, COLOR_BLACK);
	init_pair(2, COLOR_RED, COLOR_BLACK);
	init_pair(3, COLOR_YELLOW, COLOR_BLACK);
	init_pair(4, COLOR_GREEN, COLOR_BLACK);
	getmaxyx(stdscr, scrHeight, scrWidth);

	set_init_board();
	current = c_reachable.size() - 1;
	char ch;
	bool is_taken = false;
	bool mesg_status = false;
	PANEL* current_pan = c_reachable[current]->visual.get_pan();
	top_panel(current_pan);

	std::vector<PANEL*> current_hold;
	while ((ch = getch()) != 'e') {
		switch(ch) {
//			case 'q': //next panel
//				if (is_taken) {
//					std::string mesg = "a card is already being taken";
//					mesg_status = true;
//					print_bg(mesg);
//					break;
//				}
//				else if(!is_taken && mesg_status){
//					mesg_status = false;
//					clear_bg();
//				}
//				current = current > 0 ? current - 1: c_reachable.size() - 1;
//				current_pan = c_reachable[current]->visual.get_pan();
//				top_panel(current_pan);
//				break;	
//			case 'c': //choose card to operate with
//				is_taken = true;
//				break;
//			case 'v': //release card
//				is_taken = false;
//				break;
//			case 'u': //update panels
//				update_panels();
//				doupdate();
//				break;
//			case 'w':
//				if (is_taken) {
//					if (c_reachable[current]->visual.y0 <= 1)
//						break;
//					--(c_reachable[current]->visual.y0);
//					c_reachable[current]->visual.move_pan();
//				}
//				break;
//			case 's':
//				if (is_taken) {
//					if (c_reachable[current]->visual.y0 >= scrHeight - 1 - c_reachable[current]->visual.height)
//						break;
//					++(c_reachable[current]->visual.y0);
//					c_reachable[current]->visual.move_pan();
//				}
//				break;
//			case 'a':
//				if (is_taken) {
//					if (c_reachable[current]->visual.x0 <= 1)
//						break;
//					--(c_reachable[current]->visual.x0);
//					c_reachable[current]->visual.move_pan();
//				}
//				break;
//			case 'd':
//				if (is_taken) {
//					if (c_reachable[current]->visual.x0 >= scrWidth - 2 - c_reachable[current]->visual.width)
//						break;
//					++(c_reachable[current]->visual.x0);
//					c_reachable[current]->visual.move_pan();
//
//				}
//				break;
			case 'q':
				if (is_taken) {
					std::string mesg = "a card is already being taken";
					mesg_status = true;
					print_bg(mesg);
					break;
				}
				else if(!is_taken && mesg_status){
					mesg_status = false;
					clear_bg();
				}
				current = current > 0 ? current - 1: c_reachable.size() - 1;
				current_pan = c_reachable[current]->visual.get_pan();
				top_panel(current_pan);
				break;	

		}
		update_panels();
		doupdate();
	}
	endwin();
};

int IO::get_stack_y(int i) {
	return scrHeight / 3 + 2 * i;
}

int IO::get_stack_x(int i) {
	return 2 + 9 * i;
}

void IO::set_init_board() {
	elements.resize(noe);

	int cnt = 0; //already initialized

	elements[cnt].set_win_parameters(scrHeight, scrWidth, 0, 0);	//background border
	elements[cnt].get_pan();
	cnt++;

	int deck_y, deck_x;
	deck_y = 1;
	deck_x = 2;
	elements[cnt].set_win_parameters(8, 8, deck_y, deck_x);
   	elements[cnt].get_pan();	//deck border
	cnt++;

	int first_dome_y = 1;
	int first_dome_x = scrWidth * 2/3;
	for (int i = 0; i < n_out; ++i) { // domes borders
		elements[cnt].set_win_parameters(8, 8, first_dome_y, first_dome_x + 9*i);
		elements[cnt].get_pan();
		cnt++;
	}

	int initialized_cards = 0;
	c_stacks.resize(n_stacks);

	int noc_in_stacks = n_stacks * (n_stacks + 1) / 2; // enter manually: fix!!!!!!
	cards.resize(noc);

	for (int i = 0; i < n_stacks; ++i) { //stacks
		bool is_highest = false;
		bool color; // 0 - red, 1 - black
		for (int j = 0; j <= i; ++j) {
			bool is_dublicate = true;
			if (j == i)
				is_highest = true;
			else
				is_highest = false;
		 	while (is_dublicate) {
				int seed = std::rand();
				is_dublicate = false;
				E_STATUS temp_status;
				if (is_highest)
					temp_status = E_STATUS_STACK_UP;
				else
					temp_status = E_STATUS_STACK_DOWN;
				Card temp(temp_status, E_SUIT(seed % 4), E_VALUE(seed % 13), get_stack_y(j), get_stack_x(i));
				for (Card it : cards) {
					if (it == temp) {
						is_dublicate = true;
						break;
					}
				}
				if (j != 0 && (int(temp.get_suit()) / 2 == color)) {
					is_dublicate = true;
					continue;	
				}
				if (!is_dublicate) {
					if (j == 0)
						color = int(temp.get_suit()) / 2;
					else
						color = color == 0 ? 1 : 0;
					cards[initialized_cards] = temp;
					c_stacks[i].push_back(&cards[initialized_cards]);
					if (is_highest) {
						c_reachable.push_back(&cards[initialized_cards]); ///////////////////
					}
					cards[initialized_cards].visual.get_pan();
					initialized_cards++;		
				}
			}
		}
	}

	int cards_in_deck = noc - noc_in_stacks;
	for (int i = 0; i < cards_in_deck; ++i) { //deck
		bool is_dublicate = true;
	 	while (is_dublicate) {
			int seed = std::rand();
			is_dublicate = false;
			Card temp(E_STATUS_DECK, E_SUIT(seed % 4), E_VALUE(seed % 13), deck_y, deck_x);
			for (Card it : cards) {
				if (it == temp) {
					is_dublicate = true;
					break;	
				}
			}
			if (is_dublicate == false)
				cards[initialized_cards] = temp;
		}

		c_deck.push_back(&cards[initialized_cards]);
		if (i >= cards_in_deck - n_deck_shown)
			c_reachable.push_back(&cards[initialized_cards]);
		cards[initialized_cards].visual.get_pan();

		if (i < cards_in_deck - n_deck_shown) {
			hide_panel(cards[initialized_cards].visual.get_pan());
		}
		initialized_cards++;	
	}
	update_panels();
	doupdate();
}

void IO::print_bg(std::string _mesg) {
	if (elements.empty())
		std::cerr << "No background initialized" << std::endl;
	const char* chmesg = _mesg.c_str();
	mvwprintw(elements[0].win, scrHeight - 4, scrWidth - _mesg.size() - 2, chmesg);
}

void IO::clear_bg() {
	if (elements.empty())
		std::cerr << "No background initialized" << std::endl;
	werase(elements[0].get_win());
	elements[0].create_win();
	elements[0].del_pan();
	elements[0].get_pan(); //fix
	wrefresh(elements[0].get_win());
	bottom_panel(elements[0].get_pan());
	update_panels();
	doupdate();
}

IO::~IO() {

}
