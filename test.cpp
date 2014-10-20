
#include <daw/filtered_range.h>
#include <cstdlib>
#include <iostream>


int main( int argc, char **arv ) {
	std::vector<int> test_values = { 100, 1, 2, 3, 3, 4, 5, 6, 7, 8, 9, 10, 10, 11, 55, 3, 7, 22, 11 };
	
	daw::range::make_filtered_range( test_values ).where( []( const int& x ) {
		return 0 == x % 2;
	} ).stable_unique( ).for_each( []( int x ) { 
		std::cout << x << "\n";
	} );
	
	system( "PAUSE" );
	return EXIT_SUCCESS;
}


