
#include "daw/filtered_range.h"
#include <cstdlib>
#include <iostream>


int main( int argc, char **arv ) {
	std::vector<int> test_values = { 100, 1, 2, 3, 3, 4, 5, 6, 7, 8, 9, 10, 10, 11, 55, 3, 7, 22, 11 };
	std::vector<int> test_values2 = { 200, 201, 201, 199, 220, 100 };
	
	using namespace daw::range;
	auto tmp = create_filtered_range( test_values )
		.where( logic_not( logic_or( is_equal( 6 ), is_equal( 4 ) ) ) )
		.append( test_values2.begin( ), test_values2.end( ) )
		.stable_unique( ).for_each( display_item<int>( ) )
		.call([]( const daw::range::FilteredRange<int>& ) { std::cout << "-----\n"; })
		.sort( ).for_each( display_item<int>( ) );
	
	std::cout << "has 5 " << tmp.contains( 5 ) << "\n";
	std::cout << "has 4 " << tmp.contains( 4 ) << "\n";

	std::cout << "---------\n";
	daw::range::create_filtered_range( test_values ).append( test_values2.begin( ), test_values2.end( ) ).duplicates( ).for_each( display_item<int>( ) );

	system( "PAUSE" );
	return EXIT_SUCCESS;
}


