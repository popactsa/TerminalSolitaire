#include "IO.h"

#define debug 	std::string mesg_debug = "debug"; print_bg(mesg_debug); update_panels(); doupdate();

IO::IO(int _n_stacks, int _n_out, int _n_deck_shown, int _noc, int _noe): n_stacks{_n_stacks}, n_out{_n_out}, n_deck_shown{_n_deck_shown}, noc{_noc}, noe{_noe} {
	setlocale(LC_ALL, "");
	std::srand(std::time(nullptr));

	n_decks = (noc - n_stacks * (n_stacks + 1) / 2) / n_deck_shown;
	if ((noc - n_stacks * (n_stacks + 1) / 2) % n_deck_shown == 0) {
		--n_decks;
	}

	initscr();
	cbreak();
	noecho();
	start_color();
	keypad(stdscr, true);
	raw();
	refresh();

	init_pair(1, COLOR_GREEN, COLOR_BLACK);
	init_pair(2, COLOR_RED, COLOR_BLACK);
	init_pair(3, COLOR_YELLOW, COLOR_BLACK);
	init_pair(4, COLOR_GREEN, COLOR_BLACK);
	getmaxyx(stdscr, scrHeight, scrWidth);

	set_init_board();
	current = 0;
	char ch;
	bool is_taken = false;
	bool mesg_status = false;
	PANEL* current_pan = c_reachable[current]->visual.get_pan();
	top_panel(current_pan);

	update_panels();
	doupdate();

	std::vector<Card*> current_hold;
	std::array<int, 3> prev_place;
	
	while ((ch = getch())!= 'e') {
		if (mesg_status) {
			clear_bg();
			mesg_status = true;
		}
		if (ch == 'q') {
			if (!is_taken) {
				swap_deck_top();
			}		
		}
		else if (ch == 'a') {
			if (mesg_status) {
				clear_bg();
				mesg_status = false;
			}
			if (is_taken) {
				std::string mesg = "A pack of cards are taken already";
				print_bg(mesg);
				mesg_status = true;
			}
			else {
				bool is_nullptr = true;
				while (is_nullptr == true) {
					current = current > 0 ? current - 1 : c_reachable.size() - 1;
					is_nullptr = c_reachable[current] == nullptr ? true : false;
				}
				current_pan = c_reachable[current]->visual.get_pan();
				top_panel(current_pan);
			}
		}
		else if (ch == 'd') {
			if (mesg_status) {
				clear_bg();
				mesg_status = false;
			}
			if (is_taken) {
				std::string mesg = "A pack of cards are taken already";
				print_bg(mesg);
				mesg_status = true;
			}
			else {
				bool is_nullptr = true;
				while (is_nullptr == true) {
					current = current < c_reachable.size() ? current + 1 : 0;
					is_nullptr = c_reachable[current] == nullptr ? true : false;
				}
				current_pan = c_reachable[current]->visual.get_pan();
				top_panel(current_pan);
			}
		}
		else if (ch == 'c') {
			if (is_taken == false) {
				if (c_reachable[current]->get_status() == E_STATUS_STACK_UP) {
					get_stack_group(&current_hold);
					is_taken = true;
				}
				else if (c_reachable[current]->get_status() == E_STATUS_DECK_UP) {
					current_hold.push_back(c_reachable[current]);
					is_taken = true;
				}
				else if (c_reachable[current]->get_status() == E_STATUS_OUT_UP) {
					current_hold.push_back(c_reachable[current]);
					is_taken = true;
				}
				std::string mesg = "Cards are taken";
				print_bg(mesg);
			}
			else {
				std::string mesg = "A pack of cards are taken already";
				print_bg(mesg);
				mesg_status = true;
			}
		}
		else if (ch == 'v') {
			if (is_taken == true) {
				current_hold.resize(0);
				clear_bg();
				mesg_status = false;
				is_taken = false;
			}
		}
		else if (ch == 'w') {
			if (!is_taken && c_reachable[current]->get_status() == E_STATUS_STACK_UP) {
				Card* next;
				prev_place = get_place_in_stack(c_reachable[current]);
				if (prev_place[2] == int(E_STATUS_STACK_UP)) {
					if (prev_place[1] == 0)
						next = c_stacks[prev_place[0]][c_stacks[prev_place[0]].size() - 1];
					else
						next = c_stacks[prev_place[0]][prev_place[1] - 1];
				}
				else if (prev_place[2] == int(E_STATUS_DECK_UP)) {
					if (prev_place[1] == 0)
						next = c_deck[prev_place[0]][c_deck[prev_place[0]].size() - 1];
					else
						next = c_deck[prev_place[0]][prev_place[1]];
					next->up_card();
					//next->down_card();
				}

				if (next->get_status() == E_STATUS_STACK_UP || next->get_status() == E_STATUS_DECK_UP || next->get_status() == E_STATUS_OUT_UP)
					c_reachable[current] = next;

				current_pan = c_reachable[current]->visual.get_pan();
				top_panel(current_pan);	
			}
		}
		else if (ch == 's') {
			if (!is_taken && c_reachable[current]->get_status() == E_STATUS_STACK_UP) {
				Card* next;
				prev_place = get_place_in_stack(c_reachable[current]);
				if (prev_place[1] == c_stacks[prev_place[0]].size() - 1)
					next = c_stacks[prev_place[0]][0];
				else
					next = c_stacks[prev_place[0]][prev_place[1] + 1];	
				if (next->get_status() == E_STATUS_STACK_UP)
					c_reachable[current] = next;
				current_pan = c_reachable[current]->visual.get_pan();
				top_panel(current_pan);	
			}
		}
		else if (ch >= '1' && ch <= '7') {
			if (is_taken) {
				bool status = move_held(&current_hold, int(ch - '1'), &mesg_status, E_STATUS_STACK_UP);
				if (status) {
					clear_bg();
					mesg_status = false;
					is_taken = false;
				}
			}
			else {
				std::string mesg = "Pick up card first";
				print_bg(mesg);
				mesg_status = true;
			}
		}
		else if (ch == 'r') { // automatically tries to find appropriate place
			if (is_taken) {
				bool status = move_held(&current_hold, 0, &mesg_status, E_STATUS_OUT_UP); //value of stack = 0 is useless
				if (status) {
					clear_bg();
					mesg_status = false;
					is_taken = false;
				}
			}
			else {
				std::string mesg = "Pick up card first";
				print_bg(mesg);
				mesg_status = true;
			}		
		}
		update_panels();
		doupdate();
	}
	endwin();
};

void IO::swap_deck_top() {
	int cr_last = n_stacks;
	if (c_deck.empty()) {
		c_reachable[cr_last] = nullptr;
	}
	else {
		int current_deck = get_place_in_stack(c_reachable[cr_last])[0];
		int next_deck = current_deck == 0 ? c_deck.size() - 1 : current_deck - 1;

		if (c_deck[current_deck].empty()){
			c_deck.erase(c_deck.begin() + current_deck);
			next_deck = next_deck > current_deck ? next_deck - 1 : next_deck;
		}
		else {
			for (int i = 0; i < c_deck[current_deck].size(); ++i) {
				c_deck[current_deck][i]->down_card();
				hide_panel(c_deck[current_deck][i]->visual.get_pan());
			}
		}
	
		if (c_deck.empty()) {
			c_reachable[cr_last] = nullptr;
		}	
		else {	
			for (int i = 0; i < c_deck[next_deck].size(); ++i) {
				c_deck[next_deck][i]->up_card();
				show_panel(c_deck[next_deck][i]->visual.get_pan());
			}

			c_reachable[cr_last] = c_deck[next_deck][c_deck[next_deck].size() - 1];
		}
	}

	current = cr_last;
	PANEL* current_pan = c_reachable[current]->visual.get_pan();
	top_panel(current_pan);

	std::string mesg = std::to_string(c_deck.size());	
	print_bg(mesg);
	update_panels();
	doupdate();	
}

bool IO::move_held(std::vector<Card*>* _current_hold, int _stack, bool *mesg_status, E_STATUS _to_where) {
	std::array<int, 3> prev_place;
	prev_place = get_place_in_stack(_current_hold[0][0]);

	if (_to_where == E_STATUS_OUT_UP) {
		if (_current_hold->size() != 1) {
			std::string mesg = "Choose only ONE card";
			print_bg(mesg);
			*mesg_status = true;
			return false;	
		}
		else {
			int i_suit = int(_current_hold[0][0]->get_suit());
			int i_value = int(_current_hold[0][0]->get_value());

			if (c_out[i_suit].empty()){
				if (i_value == int(E_VALUE_A)) {
					_stack = i_suit;
				}
				else {
					std::string mesg = "Can't place it in suit's dome";
					*mesg_status = true;
					print_bg(mesg);
					return false;
				}
			}
			else {
				if (i_value - int(c_out[i_suit][c_out[i_suit].size() - 1]->get_value()) == 1) {
					_stack = i_suit;
				}
				else {
					std::string mesg = "Can't place it in suit's dome";
					*mesg_status = true;
					print_bg(mesg);
					return false;
				}
			}
		}
	}
	else if (_to_where == E_STATUS_STACK_UP) {
		if (prev_place[2] == E_STATUS_STACK_UP && prev_place[0] == _stack) {
			return false;
		} 
	}

	bool status = place_held_in_stack(_current_hold, _stack, mesg_status, _to_where);

	if (status == false)
		return false;

	for (int i = 0; i < _current_hold->size(); ++i) {
		if (prev_place[2] == int(E_STATUS_STACK_UP)) {
			c_stacks[prev_place[0]].pop_back();
		}
		else if (prev_place[2] == int(E_STATUS_DECK_UP)) {
			c_deck[prev_place[0]].pop_back();
		}
		else if (prev_place[2] == int(E_STATUS_OUT_UP)) {
			c_out[prev_place[0]].pop_back();
		}
	}

	if (prev_place[2] == int(E_STATUS_STACK_UP)) {
		if (prev_place[1] == 0) {
			c_reachable[current] = nullptr;
		}
		else {
			c_stacks[prev_place[0]][c_stacks[prev_place[0]].size() - 1]->up_card();
			c_reachable[current] = c_stacks[prev_place[0]][c_stacks[prev_place[0]].size() - 1];
		}
	}
	else if (prev_place[2] == int(E_STATUS_OUT_UP)) {
		if (c_out[prev_place[0]].empty()) {
			c_reachable[current] = nullptr;
		}
		else {
			show_panel(c_out[prev_place[0]][c_out[prev_place[0]].size() - 1]->visual.get_pan());
			c_out[prev_place[0]][c_out[prev_place[0]].size() - 1]->up_card();
			c_reachable[current] = c_out[prev_place[0]][c_out[prev_place[0]].size() - 1];
		}
	}
	else if (prev_place[2] == int(E_STATUS_DECK_UP)) {
		if (c_deck.empty()) {
			c_reachable[current] = nullptr;
		}
		else {
			if (prev_place[1] == 0) {
				c_deck.erase(c_deck.begin() + prev_place[0]);
				swap_deck_top();
			}
			else {
				c_reachable[current] = c_deck[prev_place[0]][prev_place[1] - 1];
			}
		}
	}
     
        _current_hold->resize(0);	
        return status;
}

bool IO::place_held_in_stack(std::vector<Card*>* _current_hold, int _stack, bool *mesg_status, E_STATUS _to_where) {
	if (_to_where == E_STATUS_OUT_UP) {
		if (c_out[_stack].empty()) {
			int cnt = 0;
			int x = get_dome_x(_stack);
			int y = get_dome_y(_stack);
	 		for (Card* it : *_current_hold) {
	 			it->visual.set_coords(y, x);
		 		it->visual.move_pan();
				it->set_status(E_STATUS_OUT_UP);
				c_out[_stack].push_back(it);
	 			cnt++;
	 		}
			c_reachable[n_stacks + 1 + _stack] = c_out[_stack][c_out[_stack].size() - 1];
			return true;
		}
		else {
			c_out[_stack][c_out[_stack].size() - 1]->down_card();
			hide_panel(c_out[_stack][c_out[_stack].size() - 1]->visual.get_pan());		

			int x = get_dome_x(_stack);
			int y = get_dome_y(_stack);
			int cnt = 1;

			for (Card* it : *_current_hold) {
				it->visual.set_coords(y, x);
				it->visual.move_pan();
				it->set_status(E_STATUS_OUT_UP);	
				c_out[_stack].push_back(it);
				cnt++;
			}
		
			c_reachable[n_stacks + 1 + _stack] = c_out[_stack][c_out[_stack].size() - 1];	
			return true;
		}	
	}
	else if (_to_where == E_STATUS_STACK_UP) {
		if (c_stacks[_stack].empty()) {
			int cnt = 0;
			int y = get_stack_y(0);
			int x = get_stack_x(_stack);
	 		for (Card* it : *_current_hold) {
	 			it->visual.set_coords(y + 2 * cnt, x);
		 		it->visual.move_pan();
				it->set_status(E_STATUS_STACK_UP);
				c_stacks[_stack].push_back(it);
	 			cnt++;
	 		}
			c_reachable[_stack] = c_stacks[_stack][c_stacks[_stack].size() - 1];
			update_stack_highest_ptr(_stack);
			return true;
		}
		else {
			if (!check_compatibility(c_stacks[_stack][c_stacks[_stack].size() - 1], _current_hold[0][0])) {
				std::string mesg = "Can't place it here";
				print_bg(mesg);
				*mesg_status = true;
				return false;
			}
		
			int x = c_stacks[_stack][c_stacks[_stack].size() - 1]->visual.x0;
			int y = c_stacks[_stack][c_stacks[_stack].size() - 1]->visual.y0;
			int cnt = 1;
			for (Card* it : *_current_hold) {
				it->visual.set_coords(y + 2 * cnt, x);
				it->visual.move_pan();
				it->set_status(E_STATUS_STACK_UP);	
				c_stacks[_stack].push_back(it);
				cnt++;
			}
		
			update_stack_highest_ptr(_stack);
			return true;
		}
	}
	else {
		return false;
	}
}

bool IO::check_compatibility(Card* _A, Card* _B) {
	if (_A->get_color() != _B->get_color() && 
		( (int(_A->get_value()) - int(_B->get_value()) == 1 && _A->get_value() != E_VALUE_A) ||
		 (_A->get_value() == E_VALUE_A && _B->get_value() == E_VALUE_K) )
	   ) {
		return true;
	}
	else {
		return false;
	}
}

std::array<int, 3> IO::get_place_in_stack(Card* _card) {
	std::array <int, 3> result{-1, -1, -1};
	for (int i = 0; i < n_stacks; ++i) {
		for (int j = 0; j < c_stacks[i].size(); ++j) {
			if (c_stacks[i][j] == _card) {
				result[0] = i;
				result[1] = j;
				result[2] = int(E_STATUS_STACK_UP);
				return result;
			}
		}
	}

	for (int i = 0; i < c_deck.size(); ++i) {
		for (int j = 0; j < c_deck[i].size(); ++j) {
			if (c_deck[i][j] == _card) {
				result[0] = i;
				result[1] = j;
				result[2] = int(E_STATUS_DECK_UP);
				return result;
			}
		}
	}
	
	for (int i = 0; i < n_out; ++i) {
		if (c_out[i][c_out[i].size() - 1] == _card) {
			result[0] = i;
			result[1] = c_out[i].size() - 1;
			result[2] = int(E_STATUS_OUT_UP);
			return result;
		}
	}	

	if (result[0] == -1 || result[1] == -1 || result[2] == -1) {
		endwin();
		std::cerr << "Error while trying find a place of a card in stack" << std::endl;
		exit(-1);
	}
}

void IO::update_stack_highest_ptr(int _stack) {
	bool sw = false;
	for (int i = 0; i < c_reachable.size(); ++i) {
		for (int j = 0; j < c_stacks[_stack].size(); ++j) {
			if (c_reachable[i] == c_stacks[_stack][j]) {
				c_reachable[i] = c_stacks[_stack][c_stacks[_stack].size() - 1];
				sw = true;
				break;
			}
		}
		if (sw == true)
			break;
	}
}

void IO::get_stack_group(std::vector<Card*> *_group) {
	int place_in_stack = 0;
	int stack = -1;
	_group->resize(0);
	for (int i = 0; i < n_stacks; ++i) { //looking for this card in stack
		bool sw = false;
		for (Card* it : c_stacks[i]) {
			if (it == c_reachable[current]) {
				sw = true;
				break;
			}
			place_in_stack++;
		}
		if (sw) {
			stack = i;
			break;
		}
		place_in_stack = 0;
	}

	if (stack == -1) {
		endwin();
		std::cerr << "Card is not found in stacks" << std::endl;
		exit(-1);
	}

	for (int i = place_in_stack; i < c_stacks[stack].size(); ++i) { //pushing all higher cards in _group
		_group->push_back(c_stacks[stack][i]);
	}	
}

int IO::get_stack_y(int i) {
	return scrHeight / 3 + 2 * i;
}

int IO::get_stack_x(int i) {
	return 2 + 9 * i;
}

int IO::get_dome_y(int i) {
	return 1;
}

int IO::get_dome_x(int i) {
	return scrWidth * 2/3 + 9 * i;
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

	c_stacks.resize(n_stacks);
	cards.resize(noc);
	int initialized_cards = 0;	

	int noc_in_stacks = n_stacks * (n_stacks + 1) / 2;

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
					cards[initialized_cards].visual.get_pan();
					c_stacks[i].push_back(&cards[initialized_cards]);
					if (is_highest) {
						c_reachable.push_back(&cards[initialized_cards]);
					}
					initialized_cards++;
				}
			}
		}
	}

	c_deck.resize(n_decks);
	for (int i = 0; i < n_decks; ++i) { //deck
		bool sw = false;
		for (int j = 0; j < n_deck_shown; ++j) {
			bool is_dublicate = true;
		 	while (is_dublicate) {
				int seed = std::rand();
				is_dublicate = false;
				E_STATUS status;
				status = i == n_decks - 1 ? E_STATUS_DECK_UP : E_STATUS_DECK_DOWN;

				int add_x = (c_reachable[0]->visual.width + 1) * (n_deck_shown - j) + 3;

				Card temp(status, E_SUIT(seed % 4), E_VALUE(seed % 13), deck_y, deck_x + add_x);
				
				for (int k = 0; k < initialized_cards; ++k) {
					Card it = cards[k];
					if (it == temp) {
						is_dublicate = true;
						break;	
					}
				}

				if (is_dublicate == false) {
					cards[initialized_cards] = temp;
					c_deck[i].push_back(&cards[initialized_cards]);
					cards[initialized_cards].visual.get_pan();
					if (cards[initialized_cards].get_status() == E_STATUS_DECK_DOWN) {
						hide_panel(cards[initialized_cards].visual.get_pan());
					}
					initialized_cards++;
					if (initialized_cards == noc) {
						sw = true;
						break;
					}
				}
			}
		}
		if (sw) {
			break;
		}
	}

	c_reachable.push_back(c_deck[c_deck.size() - 1][c_deck[c_deck.size() - 1].size() - 1]);

	c_out.resize(n_out);
	for (int i = 0; i < n_out; ++i) {
		c_out[i].resize(0);
		c_reachable.push_back(nullptr);
	}
}

void IO::print_bg(std::string _mesg) {
	if (elements.empty())
		std::cerr << "No background initialized" << std::endl;
	const char* chmesg = _mesg.c_str();
	clear_bg();
	mvwprintw(elements[0].win, scrHeight - 4, scrWidth - _mesg.size() - 2, chmesg);
}

void IO::clear_bg(bool _to_refresh) {
	if (elements.empty())
		std::cerr << "No background initialized" << std::endl;
	werase(elements[0].get_win());
	elements[0].create_win();
	elements[0].del_pan();
	elements[0].get_pan();
	wrefresh(elements[0].get_win());
	bottom_panel(elements[0].get_pan());
	if (_to_refresh) {
		update_panels();
		doupdate();
	}
}

IO::~IO() {

}
