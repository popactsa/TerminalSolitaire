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
#include "Element.h"
#include "Card.h"



class IO {
	private:
		int scrHeight, scrWidth;
		int current; //

		const int noc; // number of cards used in game
	       	const int noe; // number of static elements
		const int n_deck_shown; // number of shown cards from deck
	        const int n_out; // number of ?domes
	       	const int n_stacks; // number of stacks(columns)

		std::vector<Element> elements;
		std::vector<Card> cards;

		//pointers to cards
		std::vector<std::vector<Card*>> c_stacks; // cards in each stack
		std::vector<std::vector<Card*>> c_out; // cards in each ?dome
		std::vector<Card*> c_deck; // cards in deck;
		std::vector<Card*> c_reachable; // cards which can be moved
	public:
		IO(int _n_stacks, int _n_out, int _n_deck_shown, int _noc, int _noe);
		int get_stack_y(int i);
		int get_stack_x(int i);
		void print_bg(std::string mesg);
		void clear_bg();
		void set_init_board();
		~IO();
};

#endif
