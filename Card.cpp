#include "Card.h"

Card::Card(E_STATUS _status, E_SUIT _suit, E_VALUE _value, int _y0, int _x0) {
	value = _value;
	suit = _suit;
	status = _status;

	set_visual_parameters(_y0, _x0);
}

E_SUIT Card::get_suit() {
	return suit;
}

void Card::set_visual_parameters(int _y0, int _x0) {
	std::map<int, chtype> suit_symbol{
		{0, 'h'},
		{1, 'd'},
		{2, 'c'},
		{3, 's'}	
	};
	std::map<int, chtype> value_symbol{
		{0, '2'},
		{1, '3'},
		{2, '4'},
		{3, '5'},
		{4, '6'},
		{5, '7'},
		{6, '8'},
		{7, '9'},
		{8, 'T'},
		{9, 'J'},
		{10, 'Q'},
		{11, 'K'},
		{12, 'A'}
	};

	visual.color_scheme = suit == E_SUIT_HEARTS || suit == E_SUIT_DIAMONDS ? 2 : 3;
	//set basic card window parameters
	visual.height = 8;
	visual.width = 8;
	visual.y0 = _y0;
	visual.x0 = _x0;
	if (status == E_STATUS_STACK_UP) {
		visual.ls = ACS_VLINE;
 		visual.rs = ACS_VLINE;
 		visual.bs = ACS_HLINE;
 		visual.ts = ACS_HLINE;
	 	visual.tl = value_symbol[value];
 		visual.tr = suit_symbol[suit];
 		visual.bl = ACS_LLCORNER;
 		visual.br = ACS_LRCORNER;
	}
	else if (status == E_STATUS_STACK_DOWN) {
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
	if (suit == _given.suit && value == _given.value && status == _given.status)
		return true;
	return false;
}

//Card::~Card() {
//	visual.~Element();
//}
