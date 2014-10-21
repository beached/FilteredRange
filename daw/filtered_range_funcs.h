#pragma once

#include <functional>

namespace daw {
	namespace range {
		// Where clauses

		template<typename value_type, typename BinaryComp = std::less<value_type>>
		std::function <bool( value_type )> is_less( value_type value, BinaryComp comp = BinaryComp( ) ) {
			return[value, comp]( const value_type& test_val ) {
				return comp( test_val, value );
			};
		}

		template<typename value_type, typename BinaryComp = std::less_equal<value_type>>
		std::function <bool( value_type )> is_less_or_equal( value_type value, BinaryComp comp = BinaryComp( ) ) {
			return[value, comp]( const value_type& test_val ) {
				return comp( test_val, value );
			};
		}

		template<typename value_type, typename BinaryComp = std::greater<value_type>>
		std::function <bool( value_type )> is_greater( value_type value, BinaryComp comp = BinaryComp( ) ) {
			return[value, comp]( const value_type& test_val ) {
				return comp( test_val, value );
			};
		}

		template<typename value_type, typename BinaryComp = std::greater_equal<value_type>>
		std::function <bool( value_type )> is_greater_or_equal( value_type value, BinaryComp comp = BinaryComp( ) ) {
			return[value, comp]( const value_type& test_val ) {
				return comp( test_val, value );
			};
		}

		template<typename value_type, typename BinaryComp = std::equal_to<value_type>>
		std::function <bool( value_type )> is_equal( value_type value, BinaryComp comp = BinaryComp( ) ) {
			return[value, comp]( const value_type& test_val ) {
				return comp( test_val, value );
			};
		}

		template<typename value_type, typename BinaryComp = std::not_equal_to<value_type>>
		std::function <bool( value_type )> is_not_equal( value_type value, BinaryComp comp = BinaryComp( ) ) {
			return[value, comp]( const value_type& test_val ) {
				return comp( test_val, value );
			};
		}

		template<typename value_type>
		std::function <bool( value_type )> logic_or( std::function <bool( value_type )> lhs, std::function <bool( value_type )> rhs ) {
			return[lhs, rhs]( const value_type& test_val ) {
				return lhs( test_val ) || rhs( test_val );
			};
		}

		template<typename value_type>
		std::function <bool( value_type )> logic_and( std::function <bool( value_type )> lhs, std::function <bool( value_type )> rhs ) {
			return[lhs, rhs]( const value_type& test_val ) {
				return lhs( test_val ) && rhs( test_val );
			};
		}

		template<typename value_type>
		std::function <bool( value_type )> logic_not( std::function <bool( value_type )> pred ) {
			return[pred]( const value_type& test_val ) {
				return !pred( test_val );
			};
		}

		template<typename value_type>
		std::function <bool( value_type )> is_even( ) {
			return[]( const value_type& test_val ) {
				return 0 == test_val % 2;
			};
		}

		template<typename value_type>
		std::function <bool( value_type )> is_odd( ) {
			return[]( const value_type& test_val ) {
				return 0 != test_val % 2;
			};
		}


		// Actions
		template<typename value_type>
		std::function<void( const value_type& )> display_item( bool new_line = true ) {
			return[new_line]( const value_type& value ) {
				std::cout << value;
				if( new_line ) {
					std::cout << "\n";
				}
			};
		}

	}	// namespace range	
}	// namespace daw
