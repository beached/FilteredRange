FilteredRange
=============

A Fluent Filtered Range.  Let's filter and perform operations on a container or range.

Example:

	auto show_int = []( int x ) {
		std::cout << x << "\n";
	};

	auto is_even = []( const int& x ) {
		return 0 == x % 2;
	};

	auto under_ten = []( const int& x ) {
		return x < 10;
	};

	auto tmp = daw::range::make_filtered_range( test_values )
		.where( is_even )
		.where( under_ten )
		.for_each( show_int ).clear_where( )
		.append( test_values2.begin( ), test_values2.end( ) )
		.stable_unique( ).for_each( show_int );

