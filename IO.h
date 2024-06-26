#ifndef IO_H
#define IO_H

#include <panel.h>
#include <vector>
#include <stdio.h>
#include <ncurses.h>
#include <locale.h>
#include <iostream>
#include <string>
#include <ctime>
#include <array>
#include <cstdlib>
#include <fstream>
#include "Element.h"
#include "Card.h"



class IO {
	private:
		int scrHeight, scrWidth;
		int current;
		int action;

		const int noc; // number of cards used in game
	       	const int noe; // number of static elements
		const int n_deck_shown; // number of shown cards from deck
	        const int n_out; // number of ?domes
	       	const int n_stacks; // number of stacks(columns)
		int n_decks; // number of decks(non-const only because of it's formula, required only for initial set)
		bool take_backs = true;

		std::vector<Element> elements;
		std::vector<Card> cards;

		std::fstream log;
		const std::string log_name;
		
		std::fstream leaderboard;
		const std::string lb_name;

		//pointers to cards
		std::vector<std::vector<Card*>> c_stacks; // cards in each stack
		std::vector<std::vector<Card*>> c_out; // cards in each ?dome
		std::vector<std::vector<Card*>> c_deck; // cards in deck;
		std::vector<Card*> c_reachable; // cards which can be moved
			//0 - (n_stacks - 1) --- stacks
			//(n_stacks) - (n_stacks + n_out - 1) --- domes
			//THE LAST ONE - deck top reachable
	public: //
		IO(int _n_stacks, int _n_out, int _n_deck_shown, int _noc, int _noe, bool _take_backs,  const std::string _log_name = "1game_log", const std::string _lb_name = "leaderboard");
		void get_stack_group(std::vector<Card*> *_group);
		void update_stack_highest_ptr(int _stack);
		std::array<int, 3> get_place_in_stack(Card* _card);
		void swap_deck_top();
		bool is_correct_open(std::fstream *_f, const std::string _name);
		bool place_held_in_stack(std::vector<Card*>* _current_hold, int _stack, bool *mesg_status, E_STATUS _to_where);
		bool check_compatibility(Card* _A, Card* _B);
		void save_state();
		bool return_state();
		void take_in_held(std::vector<Card*>* _current_hold);
		bool move_held(std::vector<Card*>* _current_hold, int _stack, bool *mesg_status, E_STATUS _to_where);
		int get_stack_y(int i);
		int get_stack_x(int i);
		int get_dome_y(int i);
		int get_dome_x(int i);
		int get_deck_y(int i);
		int get_deck_x(int i);
		void print_bg(std::string mesg);
		void clear_bg(bool _to_refresh = false);
		void set_init_board();
		void refresh_visual_order();
		void print_guide();
		~IO();
};

#endif
