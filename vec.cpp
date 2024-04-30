#include <vector>
#include <iostream>

void print_v(std::vector<int> _A) {
	std::cout << "Size is : " << _A.size() << std::endl;
	for (int i = 0; i < _A.size(); ++i) {
		if (_A[i] == )
			std::cout << "0" << " ";
		else
			std::cout << _A[i] << " ";
	}
	std::cout << std::endl;
}

int main() {
	std::vector<int> A{1, 2, 3};
	print_v(A);
	A.clear();
	A.push_back(4);
	print_v(A);
}
