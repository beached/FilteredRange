// 
// #include "daw/filtered_range.h"
// #include <cstdlib>
// #include <iostream>
// #include <list>
// #include <string>
// 
// int main( int argc, char **arv ) {
// 	std::vector<int> test_values = { 100, 1, 2, 3, 3, 4, 5, 6, 7, 8, 9, 10, 10, 11, 55, 3, 7, 22, 11 };
// 	std::vector<int> test_values2 = { 200, 201, 201, 199, 220, 100 };
// 	
// 	using namespace daw::range;
// 	auto tmp = create_filtered_range( test_values )
// 		.where( is_any_of( is_equal( 1 ), is_equal( 7 ), is_even<int>( ) ) )
// 		.append( test_values2.begin( ), test_values2.end( ) )
// 		.stable_unique( ).for_each( display_item<int>( ) )
// 		.call([]( const daw::range::FilteredRange<int>& ) { std::cout << "-----\n"; })
// 		.sort( ).for_each( display_item<int>( ) );
// 	
// 	std::cout << "has 5 " << tmp.contains( 5 ) << "\n";
// 	std::cout << "has 4 " << tmp.contains( 4 ) << "\n";
// 
// 	std::cout << "---------\n";
// 	daw::range::create_filtered_range( test_values ).append( test_values2.begin( ), test_values2.end( ) ).duplicates( ).for_each( display_item<int>( ) );
// 
// 	std::list<std::string> test_values3 = { "blah", "blah blah", "hello world", "He's the hero Gotham deserves, but not the one it needs right now. So we'll hunt him. Because he can take it. Because he's not our hero. He's a silent guardian, a watchful protector. A dark knight." };
// 
// 	auto tmp2 = create_filtered_range( test_values3 ).where( []( const std::string& value ) { return std::string::npos != value.find( "deserve" ); } ).for_each( display_item<std::string>( ) );
// 
// 	system( "PAUSE" );
// 	return EXIT_SUCCESS;
// }
// 
// 
// 

#define BOOST_TEST_MODULE FilteredRangeTest
#include <boost/test/unit_test.hpp>

#include <algorithm>
#include "daw/filtered_range.h"
#include <string>

using namespace daw::range;
using std::begin;
using std::end;

BOOST_AUTO_TEST_CASE( int_test ) {
	std::vector<int> test_values = { 100, 1, 2, 3, 3, 4, 5, 6, 7, 8, 9, 10, 10, 11, 55, 3, 7, 22, 11 };
	std::vector<int> test_values2 = { 200, 201, 201, 199, 220, 100 };

	{
		auto test_values2 = create_filtered_range( test_values ).to_vector( );
		if( test_values.size( ) != test_values2.size( ) || !std::equal( begin( test_values ), end( test_values ), begin( test_values2 ) ) ) {
			BOOST_FAIL( "create_filtered_range and/or to_vector do not work" );
		}
	}

	{
		for( auto& value : create_filtered_range( test_values ).where( is_even<int>( ) ).to_vector( ) ) {
			if( 0 != value % 2 ) {
				BOOST_FAIL( "FilteredRange.where or is_even did not work" );
			}
		}
	}

	{
		for( auto& value : create_filtered_range( test_values ).where( is_odd<int>( ) ).to_vector( ) ) {
			if( 0 == value % 2 ) {
				BOOST_FAIL( "FilteredRange.where or is_even did not work" );
			}
		}
	}
}