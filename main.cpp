
#include "Element.h"
#include "Card.h"
#include "IO.h"


int main(int argc, char* argv[]) {
	if (argc > 2) {
		std::cout << "Keys:	q - update deck top" << std::endl <<
			'\t' << "a/d - move cursor between stacks" << std::endl <<
			'\t' << "e - exit" << std::endl <<
			'\t' << "c - grab a card or a stack" << std::endl <<
		        '\t' << "v - release hand" << std::endl <<
			'\t' << "1-7 - place in first-seventh stack" << std::endl <<
			'\t' << "w/s - move in stack" << std::endl <<
			std::endl << "IN OCCURANCE OF ANY ERRORS: ./exit IN TERMINAL" << std::endl;
	}
	else {
		initscr();
		cbreak();
		noecho();
		start_color();
		keypad(stdscr, true);
		raw();
		refresh();
		IO io(7, 4, 1, 52, 3 + 1 + 4);
	}	
	return 0;
}	
