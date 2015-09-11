#ifndef _INPUT_
#define _INPUT_

#include <string>
#include <iostream>
#include <sstream>

namespace input {

template<typename T>
void number( T* number ) {
	std::string input;
	getline( std::cin, input );
	std::stringstream stream( input );
	stream >> *number;
}

}

#endif // _INPUT_
