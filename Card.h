#ifndef CARD_H
#define CARD_H

#include <map>
#include "Element.h"

enum E_SUIT {E_SUIT_HEARTS, E_SUIT_DIAMONDS, E_SUIT_CLUBS, E_SUIT_SPADES};
enum E_VALUE {E_VALUE_2, E_VALUE_3, E_VALUE_4, E_VALUE_5, E_VALUE_6, E_VALUE_7, E_VALUE_8,E_VALUE_9, E_VALUE_10, E_VALUE_J, E_VALUE_Q, E_VALUE_K, E_VALUE_A};
enum E_STATUS {E_STATUS_STACK_UP, E_STATUS_STACK_DOWN, E_STATUS_OUT_UP, E_STATUS_OUT_DOWN, E_STATUS_DECK_DOWN, E_STATUS_DECK_UP};

class Card {
	private:
		E_SUIT suit;
		E_VALUE value;
		E_STATUS status;
	public:
		Element visual;

		Card(){};
		Card(E_STATUS _status, E_SUIT _suit, E_VALUE _value, int _y0 = 1, int _x0 = 1);
		void set_visual_parameters(int _y0 = 1, int _x0 = 1);
		Card& operator = (const Card& _init);
		bool operator == (const Card& _given);
		E_SUIT get_suit();
		E_STATUS get_status();
		void set_status(E_STATUS _status);
		bool get_color();
		void up_card();
		void down_card();
		~Card(){};
};

#endif
