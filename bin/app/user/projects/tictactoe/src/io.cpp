#include "io.h"

#include <iostream>
#include <sstream>
#include <string>

int io::numberIn() {
	std::string input;
	getline( std::cin, input );
	std::stringstream stream( input );

	int number;
	stream >> number;
	return number;
}

void io::numberOut(int num) {
	std::cout << num;
}

void io::stringOut(const char* str) {
	std::cout << str;
}
