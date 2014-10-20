
#include "daw/filtered_range.h"
#include <cstdlib>
#include <iostream>


int main( int argc, char **arv ) {
	std::vector<int> test_values = { 100, 1, 2, 3, 3, 4, 5, 6, 7, 8, 9, 10, 10, 11, 55, 3, 7, 22, 11 };
	std::vector<int> test_values2 = { 200, 201, 201, 199, 220 };
	
	auto on_each = []( int x ) {
		std::cout << x << "\n";
	};

	auto is_even = []( const int& x ) {
		return 0 == x % 2;
	};

	auto tmp = daw::range::make_filtered_range( test_values ).where( is_even ).for_each( on_each ).clear_where( );
	std::cout << "---\n";
	auto tmp2 = tmp.append( test_values2.begin( ), test_values2.end( ) );
	std::cout << "---\n";
	tmp2.stable_unique( ).for_each( on_each );
	std::cout << "---\n";
	system( "PAUSE" );
	return EXIT_SUCCESS;
}


