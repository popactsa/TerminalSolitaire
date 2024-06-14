#include <iostream>
#include <array>

void print_board (std::array<std::array<char, 8>, 8> _field, bool to_show_rotations = true) {
	for (int i = 0; i < 8; ++i) {
		std::cout << " ";
		for (int j = 0; j < 8; ++j) {
			std::cout << " " << _field[i][j];
		}	
		std::cout << std::endl;
	}
	std::cout << std::endl;
	if (to_show_rotations) {
//		for (int i = 0; i < 8; ++i) {
//			std::cout << " ";
//			for (int j = 7; j >= 0; --j) {
//				std::cout << " " << _field[i][j];
//			}	
//			std::cout << std::endl;
//		}
//		std::cout << std::endl;
//
//		for (int i = 7; i >= 0; --i) {
//			std::cout << " ";
//			for (int j = 0; j < 8; ++j) {
//				std::cout << " " << _field[i][j];
//			}	
//			std::cout << std::endl;
//		}
//		std::cout << std::endl;

		for (int i = 7; i >= 0; --i) {
			std::cout << " ";
			for (int j = 7; j >= 0; --j) {
				std::cout << " " << _field[i][j];
			}	
			std::cout << std::endl;
		}
		std::cout << std::endl;
	}
	std::cout << std::endl <<  "-----------------" << std::endl;
}

void clear_field (std::array<std::array<char, 8>, 8> &_field) {
	std::array<char, 8> temp;
	temp.fill('.');
    	_field.fill(temp);
}

void clear_below (std::array<std::array<char, 8>, 8> &_field, int _y) {
	std::array<char, 8> temp;
	temp.fill('.');
    	for (int i = _y + 1; i < 8; ++i) {
		_field[i] = temp;
	}
}

bool check (std::array<std::array<char, 8>, 8> _field, int _y, int _x) {
	for (int i = 0; i < 8; ++i) {
		//horizontal and vertical check
		if (_field[_y][i] == 'Q' && _x != i) {
			return false;
		}
		if (_field[i][_x] == 'Q' && _y != i) {
			return false;
		}
	}
	//diagonal check
	int start_y = _y > _x ? _y - _x : 0;
	int end_y = _y > _x ? 7 : _y + (7 - _x);
	int start_x = _y > _x ? 0 : _x - _y;

	for (int i = 0; i <= end_y - start_y; ++i) {
		if (_field[start_y + i][start_x + i] == 'Q' && start_y + i != _y) {
		       return false;
	      	}	       
	}

	start_y = _y < (7 -_x) ? 0 : _y - (7 - _x);
	end_y = _y < (7 - _x) ? _x + _y : 7;
	start_x = _y < (7 - _x) ? _x + _y : 7;

	for (int i = 0; i <= end_y - start_y; ++i) {
		if (_field[start_y + i][start_x - i] == 'Q' && start_y + i != _y) {
		       return false;
	      	}	       
	}

	return true;
}

bool setQ (std::array<std::array<char, 8>, 8> &_field, int _y, int &_solutions_found, bool &end_reached) {
	// placing Q's in each row
	bool is_place_found = false;
	bool result;
	for (int i = 0; i < 8; ++i) {
		is_place_found = check(_field, _y, i);
		if (is_place_found) {
			_field[_y][i] = 'Q';
			if (_y == 7) {
				return true;
			}
			else {
				result = setQ(_field, _y + 1, _solutions_found, end_reached);
			}
		}
		else {
			continue;
		}
		if (!result) {
			_field[_y][i] = '.';
			continue;	
		}
		else {
			print_board(_field, false);
			_field[_y][i] = '.';
			clear_below(_field, _y);
			_solutions_found += 1;
		}
	}
	return false;
}

int main() {
	std::array<std::array<char, 8>, 8> field;
	clear_field(field);

	int solutions_found = 0;
	bool end_reached = false;
	setQ(field, 0, solutions_found, end_reached);
	std::cout << "solutions found : " << solutions_found << std::endl;	
	return 0;
}
