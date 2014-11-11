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
// 
#include <iterator>
using std::begin;
using std::end;

template<typename Container>
Container copy_of( Container container ) {
	return container;
}

template<typename Container1, typename Container2>
bool are_different( const Container1& container1, const Container2& container2 ) {
	return !std::equal( begin( container1 ), end( container1 ), begin( container2 ) );
}

#define BOOST_TEST_MODULE FilteredRangeTest
#include <boost/test/unit_test.hpp>

#include <algorithm>
#include "daw/filtered_range.h"
#include <string>

using namespace daw::range;

template<typename Container>
struct HasMutated {
	HasMutated( const Container& container ): m_container( container ) { }

	template<typename Container2>
	bool operator()( const Container2& container ) {
		return are_different( m_container, container );
	}
private:
	const Container& m_container;
};

BOOST_AUTO_TEST_CASE( int_test ) {
	const std::vector<int> test_values = { 100, 1, 2, 3, 3, 4, 5, 6, 7, 8, 9, 10, 10, 11, 55, 3, 7, 22, 11 };
	const std::vector<int> test_values2 = { 200, 201, 201, 199, 220, 100 };



	HasMutated<std::vector<int>> has_mutated( test_values );

	using std::begin;
	using std::end;

	// create_filtered_range, to_vector
	{
		auto test_vals = copy_of( test_values );
		auto tmp = create_filtered_range( test_vals ).to_vector( );
		if( are_different( test_values, tmp ) ) {
			BOOST_FAIL( "create_filtered_range and/or to_vector do not work" );
		}
		if( has_mutated( test_vals ) ) {
			BOOST_FAIL( "create_filtered_range and/or to_vector mutated the underlying container" );
		}
	}

	// copy_to
	{
		auto test_vals = copy_of( test_values );
		auto tmp_vec = std::vector<int>( test_vals.size( ), 0 );
		create_filtered_range( test_vals ).copy_to( begin( tmp_vec ), end( tmp_vec ) );
		if( are_different( test_values, tmp_vec ) ) {
			BOOST_FAIL( "copy_to did not copy values properly" );
		}
		if( has_mutated( test_vals ) ) {
			BOOST_FAIL( "create_filtered_range and/or to_vector mutated the underlying container" );
		}
	}

	// append
	{
		auto test_vals = copy_of( test_values );
		auto test_vals2 = copy_of( test_values2 );
		auto tmp = create_filtered_range( test_vals ).append( begin( test_vals2 ), end( test_vals2 ) ).to_vector( );
		auto tmp_vec = copy_of( test_values );
		auto tmp_vec2 = copy_of( test_values2 );
		tmp_vec.insert( end( tmp_vec ), begin( tmp_vec2 ), end( tmp_vec2 ) );
		if( are_different( tmp, tmp_vec ) ) {
			BOOST_FAIL( "append did not work" );
		}
		if( has_mutated( test_vals ) || are_different( test_values2, test_vals2 ) ) {
			BOOST_FAIL( "append mutated the underlying container" );
		}
	}

	// where, clear_where
	{
		auto test_vals = copy_of( test_values );
		auto tmp = create_filtered_range( test_vals );

		if( tmp.where( []( const int& value ) { return false; } ).to_vector( ).size( ) != 0 ) {
			BOOST_FAIL( "where did not function.  Should have filtered all values but some remained" );
		}
		if( tmp.where( []( const int& value ) { return true; } ).to_vector( ).size( ) != test_vals.size( ) ) {
			BOOST_FAIL( "where did not function.  Should have filtered none of the values but the sizes do not match" );
		}

		tmp = tmp.where( []( const int& value ) { return false; } );
		if( tmp.to_vector( ).size( ) != 0 ) {
			BOOST_FAIL( "where did not function.  Should have filtered all values but some remained" );
		}

		if( has_mutated( test_vals ) ) {
			BOOST_FAIL( "where has mutated the underlying container" );
		}

		tmp = tmp.clear_where( );
		if( tmp.to_vector( ).size( ) != test_vals.size( ) ) {
			BOOST_FAIL( "clear_where did not function.  Should have filtered none of the values but the sizes do not match" );
		}

		if( has_mutated( test_vals ) ) {
			BOOST_FAIL( "clear_where has mutated the underlying container" );
		}

	}

	// for_each
	{
		auto test_vals = copy_of( test_values );
		auto tmp = create_filtered_range( test_vals );
		int sum1 = 0;
		for( auto& val : test_vals ) { sum1 += val; }

		int sum2 = 0;
		tmp.for_each( [&sum2]( const int& val ) { sum2 += val; } );
		if( sum1 != sum2 ) {
			BOOST_FAIL( "for_each does not access all elements" );
		}
		if( has_mutated( test_vals ) ) {
			BOOST_FAIL( "for_each has mutated the underlying container when it should not have" );
		}

		// Test mutating elements
		tmp = create_filtered_range( test_vals );
		sum2 = 0;
		tmp.for_each( []( int& val ) { val = 1; } );
		tmp.for_each( [&sum2]( const int& val ) { sum2 += val; } );		
		if( sum2 != test_vals.size( ) ) {
			BOOST_FAIL( "for_each did not allow mutation of all values" );
		}

		auto tmp_vec = tmp.to_vector( );
		if( !has_mutated( test_vals ) ) {
			BOOST_FAIL( "for_each has not mutated the underlying container when it should have" );
		}
	}
	
	// sort
	{
		auto test_vals = copy_of( test_values );

		auto tmp_vec = copy_of( test_values );
		std::sort( begin( tmp_vec ), end( tmp_vec ) );
		
		auto tmp = create_filtered_range( test_vals ).sort( ).to_vector( );
		if( are_different( tmp, tmp_vec ) ) {
			BOOST_FAIL( "sort did not function correctly" );
		}
		if( has_mutated( test_vals ) ) {
			BOOST_FAIL( "sort has mutated the underlying container" );
		}
	}

	// stable_sort
	{
		auto test_vals = copy_of( test_values );

		auto tmp_vec = copy_of( test_values );
		std::stable_sort( begin( tmp_vec ), end( tmp_vec ) );

		auto tmp = create_filtered_range( test_vals ).stable_sort( ).to_vector( );
		if( are_different( tmp, tmp_vec ) ) {
			BOOST_FAIL( "stable_sort did not function correctly" );
		}
		if( has_mutated( test_vals ) ) {
			BOOST_FAIL( "stable_sort has mutated the underlying container" );
		}	
	}

	// unique
	{
		auto test_vals = copy_of( test_values );

		auto tmp_vec = copy_of( test_values );
		auto new_last = std::unique( begin( tmp_vec ), end( tmp_vec ) );
		tmp_vec.erase( new_last, end( tmp_vec ) );

		auto tmp = create_filtered_range( test_vals ).unique( ).to_vector( );
		if( are_different( tmp, tmp_vec ) ) {
			BOOST_FAIL( "unique did not function correctly" );
		}
		if( has_mutated( test_vals ) ) {
			BOOST_FAIL( "unique has mutated the underlying container" );
		}
	}


	// sorted_unique
	{
		auto test_vals = copy_of( test_values );

		auto tmp_vec = copy_of( test_values );
		std::sort( begin( tmp_vec ), end( tmp_vec ) );
		auto new_last = std::unique( begin( tmp_vec ), end( tmp_vec ) );
		tmp_vec.erase( new_last, end( tmp_vec ) );

		auto tmp = create_filtered_range( test_vals ).sorted_unique( ).to_vector( );
		if( are_different( tmp, tmp_vec ) ) {
			BOOST_FAIL( "sorted_unique did not function correctly" );
		}
		if( has_mutated( test_vals ) ) {
			BOOST_FAIL( "sorted_unique has mutated the underlying container" );
		}
	}

	// stable_unique
	// TODO: create test case
	{

	}

	// partition
	{
		auto is_even = []( const int& value ) { return 0 == value % 2; };
		auto test_vals = copy_of( test_values );

		auto tmp_vec = copy_of( test_values );
		std::partition( begin( tmp_vec ), end( tmp_vec ), is_even );

		auto tmp = create_filtered_range( test_vals ).partition( is_even ).to_vector( );
		if( are_different( tmp, tmp_vec ) ) {
			BOOST_FAIL( "partition did not function correctly" );
		}
		if( has_mutated( test_vals ) ) {
			BOOST_FAIL( "partition has mutated the underlying container" );
		}
	}

	// stable_partition
	{
		auto is_even = []( const int& value ) { return 0 == value % 2; };
		auto test_vals = copy_of( test_values );

		auto tmp_vec = copy_of( test_values );
		std::stable_partition( begin( tmp_vec ), end( tmp_vec ), is_even );

		auto tmp = create_filtered_range( test_vals ).stable_partition( is_even ).to_vector( );
		if( are_different( tmp, tmp_vec ) ) {
			BOOST_FAIL( "stable_partition did not function correctly" );
		}
		if( has_mutated( test_vals ) ) {
			BOOST_FAIL( "stable_partition has mutated the underlying container" );
		}
	}

	// reverse
	{
		auto test_vals = copy_of( test_values );
		auto tmp_vec = copy_of( test_values );
		std::reverse( begin( tmp_vec ), end( tmp_vec ) );

		auto tmp = create_filtered_range( test_vals ).reverse( ).to_vector( );
		if( are_different( tmp, tmp_vec ) ) {
			BOOST_FAIL( "reverse did not function correctly" );
		}
		if( has_mutated( test_vals ) ) {
			BOOST_FAIL( "reverse has mutated the underlying container" );
		}
	}

	// random_shuffle
	// TODO: add test	
	{

	}

	// replace
	{
		int old_val = 3;
		int new_val = 1000;
		
		auto tmp_vec = copy_of( test_values );
		auto num_old = std::count( begin( tmp_vec ), end( tmp_vec ), old_val );
		std::replace( begin( tmp_vec ), end( tmp_vec ), old_val, new_val );
		auto num_new = std::count( begin( tmp_vec ), end( tmp_vec ), new_val );
		assert( num_old == num_new );

		auto test_vals = copy_of( test_values );
		auto tmp = create_filtered_range( test_values ).replace( old_val, new_val ).to_vector( );
		if( num_new != std::count( begin( tmp ), end( tmp ), new_val ) ) {
			BOOST_FAIL( "replace did not function correctly" );
		}
		if( !has_mutated( test_vals ) ) {
			BOOST_FAIL( "replace has not mutated the underlying container but it should have" );
		}
	}

	// is_even
	{
		auto test_vals = copy_of( test_values );
		for( auto& value : create_filtered_range( test_vals ).where( is_even<int>( ) ).to_vector( ) ) {
			if( 0 != value % 2 ) {
				BOOST_FAIL( "FilteredRange.where or is_even did not work" );
			}
		}
	}

	// is_odd
	{
		auto test_vals = copy_of( test_values );
		for( auto& value : create_filtered_range( test_vals ).where( is_odd<int>( ) ).to_vector( ) ) {
			if( 0 == value % 2 ) {
				BOOST_FAIL( "FilteredRange.where or is_even did not work" );
			}
		}
	}
}