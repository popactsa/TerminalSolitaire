#include "IO.h"

#define debug 	std::string mesg_debug = "debug"; print_bg(mesg_debug); update_panels(); doupdate();

IO::IO(int _n_stacks, int _n_out, int _n_deck_shown, int _noc, int _noe, const std::string _log_name): n_stacks{_n_stacks}, n_out{_n_out}, n_deck_shown{_n_deck_shown}, noc{_noc}, noe{_noe}, log_name{_log_name}
{
	setlocale(LC_ALL, "");
	std::srand(std::time(nullptr));

	n_decks = (noc - n_stacks * (n_stacks + 1) / 2) / n_deck_shown;
//	if ((noc - n_stacks * (n_stacks + 1) / 2) % n_deck_shown == 0) {
//		--n_decks;
//	}

	init_pair(1, COLOR_CYAN, COLOR_BLACK);
	init_pair(2, COLOR_RED, COLOR_BLACK);
	init_pair(3, COLOR_YELLOW, COLOR_BLACK);
	init_pair(4, COLOR_CYAN, COLOR_BLACK);
	getmaxyx(stdscr, scrHeight, scrWidth);

	set_init_board();
	current = 0;
	char ch;
	bool is_taken = false;
	bool mesg_status = false;
	PANEL* current_pan = c_reachable[current]->visual.get_pan();
	top_panel(current_pan);

	refresh_visual_order();
	update_panels();
	doupdate();

	std::vector<Card*> current_hold;
	std::array<int, 3> prev_place;

	bool is_game_won = false;

	log.open(log_name, std::ios::out | std::ios::in | std::ios::trunc);
	is_correct_open(&log,log_name);	
	
	action = 0;
	bool is_changed;
	while ((ch = getch())!= 'p') {
		if (action == 0) {
			save_state();
			action++;
		}
		is_changed = false;
		if (is_game_won) {
			break;
		}
		if (mesg_status) {
			clear_bg();
			mesg_status = true;
		}
		if (ch == 'q') {
			if (current != n_stacks && !c_deck.empty()) {
				current = n_stacks;
				top_panel(c_reachable[current]->visual.get_pan());
				refresh_visual_order();
				is_changed = true;
			}
			else {
				if (!is_taken) {
					swap_deck_top();
				}
			}		
		}
		else if (ch == 'h') {
			print_guide();
		}
		else if (ch == 'a') {
			if (mesg_status) {
				clear_bg();
				mesg_status = false;
			}
			bool is_nullptr = true;
			while (is_nullptr == true) {
				current = current > 0 ? current - 1 : c_reachable.size() - 1;
				is_nullptr = c_reachable[current] == nullptr ? true : false;
			}
			current_pan = c_reachable[current]->visual.get_pan();
			top_panel(current_pan);
			refresh_visual_order();
		}
		else if (ch == 'd') {
			if (mesg_status) {
				clear_bg();
				mesg_status = false;
			}
			bool is_nullptr = true;
			while (is_nullptr == true) {
				current = current < c_reachable.size() - 1 ? current + 1 : 0;
				is_nullptr = c_reachable[current] == nullptr ? true : false;
			}
			current_pan = c_reachable[current]->visual.get_pan();
			top_panel(current_pan);	
			refresh_visual_order();
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
			take_in_held(&current_hold);
			is_taken = true;
			if (is_taken) {
				bool status = move_held(&current_hold, int(ch - '1'), &mesg_status, E_STATUS_STACK_UP);
				if (status) {
					clear_bg();
					mesg_status = false;
					is_changed = true;
				}
				is_taken = false;
			}
			else {
				std::string mesg = "Pick up card first";
				print_bg(mesg);
				mesg_status = true;
			}
		}
		else if (ch == 'r') { // automatically tries to find appropriate place
			take_in_held(&current_hold);
			is_taken = true;
			if (is_taken) {
				bool status = move_held(&current_hold, 0, &mesg_status, E_STATUS_OUT_UP); //value of stack = 0 is useless
				if (status) {
					clear_bg();
					mesg_status = false;
					is_changed = true;	
				}
				is_taken = false;
			}
			else {
				std::string mesg = "Pick up card first";
				print_bg(mesg);
				mesg_status = true;
			}		
		}
		else if (ch == 'z') {
			return_state();
			save_state();
		}

		for (int i = 0; i < n_out; ++i) {
			if (c_out[i].size() != 12) {
				is_game_won = false;
				break;
			}
		}
		if (is_game_won) {
			top_panel(elements[0].get_pan());
			std::string mesg("You won");
			print_bg(mesg);
		}
		if (is_changed) {
			save_state();
			action++;
		}
		update_panels();
		doupdate();
	}
	log.close();
	endwin();
};

bool IO::is_correct_open(std::fstream *_f, const std::string _name) {
	try{
		if (!_f->is_open()){
			throw("Can't open file");
		}
	}
	catch (const char* errorMsg){
		endwin();
		std::cerr << errorMsg << std::endl;
		std::cerr << _name << std::endl;
		exit(-1);
	}
}

bool IO::return_state() {
	if (action <= 1)
		return false;
	action--;
	std::string line;
	log.seekg(0, std::ios::beg);
	std::vector<std::string> before_log;
	for (int i = 0; i < action && std::getline(log, line); ) {
		if (line[0] == '-') {
			++i;
		}
		if (i == action)
			break;
		before_log.push_back(line);
	}

	const std::string deb = "deb";
	std::fstream fout(deb, std::ios::out);
	is_correct_open(&fout, deb);

	c_deck.clear(); c_deck.resize(n_decks);
	c_stacks.clear(); c_stacks.resize(n_stacks);
	c_out.clear(); c_out.resize(n_out);
	c_reachable.clear(); c_reachable.resize(n_stacks + 1 + n_out, nullptr);

	int cnt_ = 0;
	while(std::getline(log, line) && cnt_ < noc) {
		cnt_++;
		int cnt = 0;
		std::string temp;
		std::array<int, 6> card_state;
		// 0 - number of a card
		// 1 - in what substructure card is placed
		// 2 - card's place in substructure
		// 3 - in what structure card is placed
		// 4 - status
		// 5 - place in c_reachable
		std::string state;
		for (std::string::iterator it = line.begin(); cnt < 6; ++it) {
			if (*it == ' ' || *it == '\n') {
				card_state[cnt] = std::stoi(temp);
				state += std::to_string(card_state[cnt]) + ' ';
				cnt++;
				temp = "";
			}
			else {
				temp += *it;
			}
		}

		Card* c = &cards[card_state[0]];
		c->set_status(E_STATUS(card_state[4]));
		if (card_state[3] == int(E_STATUS_DECK_UP)) {
			if (c_deck[card_state[1]].size() - 1 < card_state[2] || c_deck[card_state[1]].empty()) {
				c_deck[card_state[1]].resize(card_state[2] + 1, nullptr);
			}
	//		c_deck[card_state[1]].resize(30, nullptr);
			c->set_visual_parameters(get_deck_y(card_state[2]), get_deck_x(card_state[2]));
			c_deck[card_state[1]][card_state[2]] = c;
			if (c->is_up()) {
				show_panel(c->visual.get_pan());
				top_panel(c->visual.get_pan());
			}

		}
		else if (card_state[3] == int(E_STATUS_OUT_UP)) {
			if (c_out[card_state[1]].size() - 1 < card_state[2] || c_out[card_state[1]].empty()) {
				c_out[card_state[1]].resize(card_state[2] + 1, nullptr);
			}
	//		c_out[card_state[1]].resize(10, nullptr);
			c->set_visual_parameters(get_dome_y(card_state[1]), get_dome_x(card_state[1]));
			c_out[card_state[1]][card_state[2]] = c;
			if (c->is_up()) {
				show_panel(c->visual.get_pan());
				top_panel(c->visual.get_pan());
			}
		}
		else if (card_state[3] == int(E_STATUS_STACK_UP)) {
			if (c_stacks[card_state[1]].size() - 1 < card_state[2] || c_stacks[card_state[1]].empty()) {
				c_stacks[card_state[1]].resize(card_state[2] + 1, nullptr);
			}
	//		c_stacks[card_state[1]].resize(10, nullptr);
			c->set_visual_parameters(get_stack_y(card_state[2]), get_stack_x(card_state[1]));
			if (c->is_up()) {
				show_panel(c->visual.get_pan());
			}	
			c_stacks[card_state[1]][card_state[2]] = c;
		}
		c->visual.move_pan();
		if (card_state[5] != -1) {
			if (c_reachable.size() - 1 < card_state[5]) {
				c_reachable.resize(card_state[5] + 1);
			}
			c_reachable[card_state[5]] = c;
			current = card_state[5];
		}
		//change current	
		c->visual.update_pan();	
		fout << state << std::endl;	
	}
	
	fout << "c_stacks size is " << c_stacks.size() << std::endl;
	for (int i = 0; i < n_stacks; ++i) {
		if (c_stacks[i].empty())
			continue;
		for (int j = c_stacks[i].size() - 1; j >= 0; --j) {
			if (c_stacks[i][j] == nullptr)
				c_stacks[i].erase(c_stacks[i].begin() + j);
		}
		if (c_stacks[i].empty())
			c_reachable[i] = nullptr;
		fout << std::endl << '\t' << i << " : " << c_stacks[i].size() << std::endl;
	}

	fout << "c_out size is " << c_out.size() << std::endl;
	for (int i = 0; i < n_out; ++i) {
		if (c_out[i].empty())
			continue;
		for (int j = c_out[i].size() - 1; j >= 0; --j) {
			if (c_out[i][j] == nullptr)
				c_out[i].erase(c_out[i].begin() + j);
		}
		if (c_out[i].empty())
			c_reachable[n_stacks + 1 + i] = nullptr;
		fout << '\t' << i << " : " << c_out[i].size() << std::endl;
	}

	fout << "c_deck size is " << c_deck.size() << std::endl;
	for (int i = 0; i < n_decks; ++i) {
		if (c_deck[i].empty())
			continue;
		for (int j = c_deck[i].size() - 1; j >= 0; --j) {
			if (c_deck[i][j] == nullptr)
				c_deck[i].erase(c_deck[i].begin() + j);
		}
		fout << '\t' << i << " : " << c_deck[i].size() << std::endl;
	}	      
	fout << "c_reachable size is " << c_reachable.size() << std::endl;

	for (int i = 0; i < c_reachable.size(); ++i) {
		if (c_reachable[i] != nullptr) {
			current = i;
			break;
		}
	}
	for (int i = 0; i < c_reachable.size(); ++i) {
		if (c_reachable[i] != nullptr) {
			top_panel(c_reachable[i]->visual.get_pan());
		}
	}
	refresh_visual_order();
	fout.close();
	log.close();
	log.clear();
	log.open(log_name, std::ios::out | std::ios::in | std::ios::trunc | std::ios::ate);
	is_correct_open(&log, log_name);
	for (std::string it : before_log) {
		log << it << std::endl;
	}
	return true;
}

void IO::save_state() {
	log << "---" << action << "---" << std::endl;
	for (int i = 0; i < noc; ++i) {
		std::array<int, 3> card_place = get_place_in_stack(&cards[i]);
		// 3rd number says only about card's place, but not about it's status(is it open or close)
		int status = cards[i].get_status();
		int place_in_c_reachable = cards[i].find_card_in_vector(c_reachable);
		// 0 - number of a card
		// 1 - in what substructure card is placed
		// 2 - card's place in substructure
		// 3 - in what structure card is placed
		// 4 - status
		// 5 - place in c_reachable	
		log << i << ' ' << card_place[0] << ' ' << card_place[1] << ' ' << card_place[2] << ' ' << status << ' ' << place_in_c_reachable << std::endl;
	}
}

void IO::take_in_held(std::vector<Card*>* _current_hold) {
	_current_hold->resize(0);
	if (c_reachable[current]->get_status() == E_STATUS_STACK_UP) {
		get_stack_group(_current_hold);
	}
	else if (c_reachable[current]->get_status() == E_STATUS_DECK_UP) {
		_current_hold->push_back(c_reachable[current]);
	}
	else if (c_reachable[current]->get_status() == E_STATUS_OUT_UP) {
		_current_hold->push_back(c_reachable[current]);
	}
}

void IO::swap_deck_top() {
	int cr_last = n_stacks;
	int next_deck, current_deck;
	if (c_deck.empty()) {
		c_reachable[cr_last] = nullptr;
	}
	else {
		current_deck = get_place_in_stack(c_reachable[cr_last])[0];
		next_deck = current_deck == 0 ? c_deck.size() - 1 : current_deck - 1;

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

	if (!c_deck.empty()) {
		current = cr_last;
	}
	else {
		for (int i = 0; i < n_stacks; ++i) {
			if (!c_stacks[i].empty()) {
				current = i;
				break;
			}
		}
	}

	PANEL* current_pan = c_reachable[current]->visual.get_pan();
	top_panel(current_pan);

	if (!c_deck.empty()) {
		std::string mesg = std::to_string(next_deck);	
		print_bg(mesg);
	}
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
			for (int i = prev_place[0]; i > 0; --i) {
				c_deck[i].insert(c_deck[i].begin(), c_deck[i-1][c_deck[i-1].size() - 1]);
				if (i == prev_place[0]) {
					c_deck[i][0]->up_card();
				}

				for (int j = c_deck[i-1].size() - 1; j > 0; --j) {
					c_deck[i-1][j] = c_deck[i-1][j - 1];
				}
				c_deck[i-1].erase(c_deck[i-1].begin());
			}
			if (c_deck[0].empty()) {
				c_deck.erase(c_deck.begin());
				if (prev_place[0] > 0) {
					c_reachable[current] = c_deck[prev_place[0] - 1][c_deck[prev_place[0] -1].size() - 1];
				}
				else {
					swap_deck_top();
				}
			}
			else {
				c_reachable[current] = c_deck[prev_place[0]][c_deck[prev_place[0]].size() - 1];
			}
			for (int i = 0; i < c_deck.size(); ++i) {
				for (int j = 0; j < c_deck[i].size(); ++j) {
					c_deck[i][j]->visual.set_coords(get_deck_y(j), get_deck_x(j));
					c_deck[i][j]->visual.move_pan(true);
				}
			}
		}
	}
    

	if (_to_where == E_STATUS_OUT_UP) {
		current = n_stacks + 1 + _stack;
	}
	else if (_to_where == E_STATUS_STACK_UP) {
		current = _stack;
	}
        _current_hold->resize(0);

	PANEL* current_pan = c_reachable[current]->visual.get_pan();
	top_panel(current_pan);

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
			if (_current_hold[0][0]->get_value() != E_VALUE_K) {
				std::string mesg = "Can't place it here";
				print_bg(mesg);
				*mesg_status = true;
				return false;	
			}
			int cnt = 0;
			int y = get_stack_y(0);
			int x = get_stack_x(_stack);
	 		for (Card* it : *_current_hold) {
	 			it->visual.set_coords(y + 1 * cnt, x);
		 		it->visual.move_pan();
				it->set_status(E_STATUS_STACK_UP);
				c_stacks[_stack].push_back(it);
	 			cnt++;
	 		}
			c_reachable[_stack] = c_stacks[_stack][c_stacks[_stack].size() - 1];
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
				it->visual.set_coords(y + 1 * cnt, x);
				it->visual.move_pan();
				it->set_status(E_STATUS_STACK_UP);	
				c_stacks[_stack].push_back(it);
				cnt++;
			}
			
			c_reachable[_stack] = c_stacks[_stack][c_stacks[_stack].size() - 1];
			return true;
		}
	}
	else {
		return false;
	}
}

bool IO::check_compatibility(Card* _A, Card* _B) {
	if (_A->get_color() != _B->get_color()/* && 
		( (int(_A->get_value()) - int(_B->get_value()) == 1) )*/
	   ) {
		return true;
	}
	else {
		return false;
	}
}

std::array<int, 3> IO::get_place_in_stack(Card* _card) {
	std::array <int, 3> result{-1, -1, -1};
	//result[2] doesn't say anything about card's status, only about it's place(dome, stacks, deck)
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
		for (int j = 0; j < c_out[i].size(); ++j) {
			if (c_out[i][j] == _card) {
				result[0] = i;
				result[1] = j;
				result[2] = int(E_STATUS_OUT_UP);
				return result;
			}
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
	for (int i = 0; i < n_stacks; ++i) {
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
	int res = scrHeight / 4 + 1*i;
	if (res < scrHeight - 6)
		return res;
	else
		return scrHeight - 6;
}

int IO::get_stack_x(int i) {
	return 2 + 8 * i;
}

int IO::get_dome_y(int i) {
	return 1;
}

int IO::get_dome_x(int i) {
	return scrWidth * 2/3 + 8 * i;
}

int IO::get_deck_y(int i) {
	return 1;
}

int IO::get_deck_x(int i) {
	return 2 + 8 * i;
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
	elements[cnt].set_win_parameters(7, 7, deck_y, deck_x);
   	elements[cnt].get_pan();	//deck border
	cnt++;

	int first_dome_y = 1;
	int first_dome_x = scrWidth * 2/3;
	for (int i = 0; i < n_out; ++i) { // domes borders
		elements[cnt].set_win_parameters(7, 7, get_dome_y(i), get_dome_x(i));
		elements[cnt].get_pan();
		cnt++;
	}

	for (int i = 0; i < n_stacks; ++i) {
		char number = static_cast<const char>(i) + '1';
		const char* number_array = new char(number);
		mvwprintw(elements[0].win, get_stack_y(0) - 1, get_stack_x(i) + (get_stack_x(1) - get_stack_x(0)) / 2 - 1, number_array);
		delete[] number_array;	
	}

	int noc_in_stacks = n_stacks * (n_stacks + 1) / 2;

	int chosen_red, chosen_black;
	for (int i = 0; i < 2; ++i) {
		int seed = std::rand();
		if (i == 0) {
			chosen_red = seed % 2;
		}
		else if (i == 1) {
			chosen_black = seed % 2 + 2;
		}
	}

	std::ofstream fout("debug_log");

	int exit_counter = 0;
	int initialized_cards = 0;
	bool retry_status = true;
	while (exit_counter < 20 && retry_status) {
		retry_status = false;
		std::vector<Card> chosen;
		for (int i = 0; i < n_stacks + n_decks * n_deck_shown; ++i) {
			if (i < 13) {
				Card temp(E_SUIT(chosen_red), E_VALUE(i % 13));
				chosen.push_back(temp);
			}
			else if (i >= 13 && i < 26) {
				Card temp(E_SUIT(chosen_black), E_VALUE(i % 13));
				chosen.push_back(temp);
			}
			else {
				int seed = std::rand();
				Card temp(E_SUIT(seed % 4), E_VALUE(seed % 13));
				bool is_dublicate = false;
				for (int j = 0; j < i; ++j) {
					if (chosen[j] == temp)
						is_dublicate = true;
				}	
				
				if (is_dublicate) {
					--i;
				}
				else {
					chosen.push_back(temp);
				}
			}
		}		
		c_stacks.resize(n_stacks);
		c_deck.resize(n_decks);
		cards.resize(noc);

		for (int i = 0; i < n_stacks; ++i) {
			int seed = std::rand() % chosen.size();
			cards[initialized_cards] = chosen[seed];
			c_stacks[i].resize(i + 1);
			c_stacks[i][i] = &cards[initialized_cards];
			initialized_cards++;
			c_reachable.push_back(c_stacks[i][i]);
			chosen.erase(chosen.begin() + seed);
		}
	
		for (int i = 0; i < n_decks * n_deck_shown; ++i) {
			int seed = std::rand() % chosen.size();
			cards[initialized_cards] = chosen[seed];
			if (i / n_deck_shown == n_decks - 1) {
				cards[initialized_cards].set_status(E_STATUS_DECK_UP);
			}
			else {
				cards[initialized_cards].set_status(E_STATUS_DECK_DOWN);	
			}
	
			cards[initialized_cards].set_visual_parameters(get_deck_y(i % n_deck_shown), get_deck_x(i % n_deck_shown));
	
			if (i % n_deck_shown == 0) {
				c_deck[i / n_deck_shown].resize(n_deck_shown);
			}
			c_deck[i / n_deck_shown][i % n_deck_shown] = &cards[initialized_cards];
			//PANEL* pan = cards[initialized_cards].visual.get_pan(); 
		
	//		if (i / n_deck_shown == n_decks - 1) {
	//			top_panel(pan);
	//		}
	//		else {
	//			hide_panel(pan);
	//		}
			initialized_cards++;
			chosen.erase(chosen.begin() + seed);
		}
		c_reachable.push_back(c_deck[c_deck.size() - 1][c_deck[c_deck.size() - 1].size() - 1]);

		int counter = 0;
		for (int i = 0; i < n_stacks; ++i) { //stacks
			bool is_highest = false;
			bool color; // 0 - red, 1 - black
			for (int j = i; j >= 0; --j) {
				bool is_dublicate = true;
				if (j == i)
					is_highest = true;
				else
					is_highest = false;
				counter = 0;
			 	while (is_dublicate) {
					if (counter > 200) {
						exit_counter++;
						retry_status = true;
						break;
					}
					int seed = std::rand();
					is_dublicate = false;
					E_STATUS temp_status;
	
					if (is_highest) { //already initialized
						temp_status = E_STATUS_STACK_UP;
						c_stacks[i][j]->set_status(temp_status);
						c_stacks[i][j]->set_visual_parameters(get_stack_y(j), get_stack_x(i));
						color = c_stacks[i][j]->get_color();
						//PANEL* pan = c_stacks[i][j]->visual.get_pan();
						//top_panel(pan);
					}
					else {
						temp_status = E_STATUS_STACK_DOWN;
						Card temp(temp_status, E_SUIT(seed % 4), E_VALUE(seed % 13), get_stack_y(j), get_stack_x(i));
						
						for (int k = 0; k < initialized_cards; ++k) {
							if (temp == cards[k]) {
							//	fout << initialized_cards << " " << k << std::endl;
								is_dublicate = true;
								break;
							}
						}
						if (temp.get_color() == color) {
							is_dublicate = true;
							continue;	
						}	
						if (!is_dublicate) {
							color = color == 0 ? 1 : 0;
							cards[initialized_cards] = temp;
							c_stacks[i][j] = &cards[initialized_cards];
							//PANEL* pan = c_stacks[i][j]->visual.get_pan();
							//top_panel(pan);
							initialized_cards++;
							break;
						}
						counter++;
					}
				}
				if (retry_status) {
					break;
				}
			}
			if (retry_status) {
				initialized_cards = 0;
				c_stacks.clear();
				c_deck.clear();
				cards.clear();
				c_reachable.clear();
				break;
			}
		}
	}
	if (retry_status) {
		endwin();
		exit(-1);
	}
	else {
		for (int i = 0; i < n_deck_shown; ++i) {
			PANEL* pan = c_deck[c_deck.size() - 1][i]->visual.get_pan();
			top_panel(pan);
		}
		for (int i = 0; i < n_stacks; ++i) {
			for (int j = i; j >= 0; --j) {
				PANEL* pan = c_stacks[i][j]->visual.get_pan();
				top_panel(pan);
			}
		}
	}

	c_out.resize(n_out);
	for (int i = 0; i < n_out; ++i) {
		c_out[i].resize(0);
		c_reachable.push_back(nullptr);
	}
	std::array<int, 4> counts = {0, 0, 0, 0};
	for (int i = 0; i < initialized_cards; ++i) {
		counts[cards[i].get_suit()]++;
		fout << i << " " << cards[i].get_suit() << " " << cards[i].get_value() << std::endl;
	}
	fout << std::endl << initialized_cards << std::endl;
	fout << counts[0] << " " << counts[1] << " " << counts[2] << " " << counts[3] << std::endl;
	fout.close();
}

void IO::refresh_visual_order() {
	for (int i = 0; i < n_stacks; ++i) {
		if (c_stacks[i].empty())
			continue;
		for (int j = 0; j < c_stacks[i].size(); ++j) {
			top_panel(c_stacks[i][j]->visual.get_pan());
		}
	}
	std::array<int, 3> place = get_place_in_stack(c_reachable[current]);
	PANEL* pan;
	if (place[2] == E_STATUS_DECK_UP) {
		pan = c_deck[place[0]][place[1]]->visual.get_pan();
	}
	else if (place[2] == E_STATUS_OUT_UP) {
		pan = c_out[place[0]][place[1]]->visual.get_pan();
	}
	else if (place[2] == E_STATUS_STACK_UP) {
		pan = c_stacks[place[0]][place[1]]->visual.get_pan();
	}
	top_panel(pan);
}

void IO::print_guide() {
	std::string mesg = "q - swap deck; 1-7 - place in stack, r - place in dome, w/s - move in stack";
	print_bg(mesg);
}

void IO::print_bg(std::string _mesg) {
	if (elements.empty())
		std::cerr << "No background initialized" << std::endl;
	const char* chmesg = _mesg.c_str();
	clear_bg();
	//mvwprintw(elements[0].win, scrHeight - 4, scrWidth - _mesg.size() - 2, chmesg);
	mvwprintw(elements[0].win, 4, get_dome_x(0) - _mesg.size() - 2, chmesg);
}

void IO::clear_bg(bool _to_refresh) {
	if (elements.empty())
		std::cerr << "No background initialized" << std::endl;
	std::string mesg(35, ' ');
	const char* chmesg = mesg.c_str();
	//mvwprintw(elements[0].win, scrHeight - 4, scrWidth - mesg.size() - 2, chmesg);
	mvwprintw(elements[0].win, 4, get_dome_x(0) - mesg.size() - 2, chmesg);
//	werase(elements[0].get_win());
//	elements[0].create_win();
//	elements[0].del_pan();
//	elements[0].get_pan();
//	wrefresh(elements[0].get_win());
//	bottom_panel(elements[0].get_pan());
	if (_to_refresh) {
		update_panels();
		doupdate();
	}
}

IO::~IO() {

}
