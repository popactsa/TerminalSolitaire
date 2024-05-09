#include "Card.h"

Card::Card(E_STATUS _status, E_SUIT _suit, E_VALUE _value, int _y0, int _x0) {
	value = _value;
	suit = _suit;
	status = _status;

	set_visual_parameters(_y0, _x0);
}

Card::Card(E_SUIT _suit, E_VALUE _value) {
	value = _value;
	suit = _suit;
}

E_SUIT Card::get_suit() {
	return suit;
}

E_STATUS Card::get_status() {
	return status;
}

void Card::set_status(E_STATUS _status) {
	status = _status;
}

void Card::set_visual_parameters(int _y0, int _x0) {
	std::map<int, chtype> suit_symbol{
		{0, 'h'},
		{1, 'd'},
		{2, 'c'},
		{3, 's'}	
	};
	std::map<int, chtype> value_symbol{
		{0, 'A'},
		{1, '2'},
		{2, '3'},
		{3, '4'},
		{4, '5'},
		{5, '6'},
		{6, '7'},
		{7, '8'},
		{8, '9'},
		{9, 'T'},
		{10, 'J'},
		{11, 'Q'},
		{12, 'K'}
	};

	visual.color_scheme = suit == E_SUIT_HEARTS || suit == E_SUIT_DIAMONDS ? 2 : 3;
	//set basic card window parameters
	visual.height = 6;
	visual.width = 6;
	visual.y0 = _y0;
	visual.x0 = _x0;
	if (status == E_STATUS_STACK_UP || status == E_STATUS_DECK_UP || status == E_STATUS_OUT_UP) {
		visual.ls = ACS_VLINE;
 		visual.rs = ACS_VLINE;
 		visual.bs = ACS_HLINE;
 		visual.ts = ACS_HLINE;
	 	visual.tl = value_symbol[value];
 		visual.tr = suit_symbol[suit];
 		visual.bl = ACS_LLCORNER;
 		visual.br = ACS_LRCORNER;
	}
	else if (status == E_STATUS_STACK_DOWN || status == E_STATUS_OUT_DOWN || status == E_STATUS_DECK_DOWN) {
		visual.ls = ACS_CKBOARD;
 		visual.rs = ACS_CKBOARD;
 		visual.bs = ACS_CKBOARD;
 		visual.ts = ACS_CKBOARD;
	 	visual.tl = ACS_CKBOARD;
 		visual.tr = ACS_CKBOARD;
 		visual.bl = ACS_CKBOARD;
 		visual.br = ACS_CKBOARD;
		visual.color_scheme = 4;	
	}
}

Card& Card::operator = (const Card& _init) {
	suit = _init.suit;
	value = _init.value;
	status = _init.status;
	visual = _init.visual;
	return *this;
}

bool Card::operator == (const Card& _given) {
	if (suit == _given.suit && value == _given.value)
		return true;
	return false;
}

bool Card::get_color() {
	if (int(suit) / 2 == 0)
		return 0; // red
	else
		return 1; // black
}

E_VALUE Card::get_value() {
	return value; //
}

void Card::up_card() {
	if (status == E_STATUS_STACK_DOWN)
		status = E_STATUS_STACK_UP;
	else if (status == E_STATUS_OUT_DOWN)
		status = E_STATUS_OUT_UP;
	else if (status == E_STATUS_DECK_DOWN)
		status = E_STATUS_DECK_UP;

	set_visual_parameters(visual.y0, visual.x0);
	visual.update_pan();
}

void Card::down_card() {
	if (status == E_STATUS_STACK_UP)
		status = E_STATUS_STACK_DOWN;
	else if (status == E_STATUS_OUT_UP)
		status = E_STATUS_OUT_DOWN;
	else if (status == E_STATUS_DECK_UP)
		status = E_STATUS_DECK_DOWN;

	set_visual_parameters(visual.y0, visual.x0);
	visual.update_pan();}
//Card::~Card() {
//	visual.~Element();
//}
