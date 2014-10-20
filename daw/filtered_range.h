#pragma once

#include <algorithm>
#include <boost/iterator/filter_iterator.hpp>
#include <exception>
#include <functional>
#include <iterator>
#include <memory>
#include <set>
#include <stdexcept>
#include <vector>

namespace daw {
	namespace range {
		template<typename value_type>
		class FilteredRange {
		public:			

			template<typename Iter>
			FilteredRange( Iter first_inclusive, Iter last_exclusive ): m_value_refs( first_inclusive, last_exclusive ), m_pred_include( ) { }

			FilteredRange& operator=(FilteredRange rhs) {
				m_value_refs = std::move( rhs.m_value_refs );
				m_pred_include = std::move( rhs.m_pred_include );
				return *this;
			}

			FilteredRange( FilteredRange&& other ): m_value_refs( std::move( other.m_value_refs ) ), m_pred_include( std::move( other.m_pred_include ) ) { }
			FilteredRange( ) = delete;
			FilteredRange( const FilteredRange& ) = default;

			virtual ~FilteredRange( ) = default;

			using predicate_type = std::function < bool( value_type ) >;

			//////////////////////////////////////////////////////////////////////////
			/// Summary: Adds a predicate that when false for a value 
			/// filters out the value.  
			FilteredRange where( predicate_type predicate ) const {
				auto result = copy_of_me( );
				result.m_pred_include.push_back( predicate );
				return result;
			}
			
			//////////////////////////////////////////////////////////////////////////
			/// Summary: Clears all where predicates
			FilteredRange clear_where( ) const {
				auto result = copy_of_me( ).do_filter( );
				result.m_pred_include.clear( );
				return result;
			}

			//////////////////////////////////////////////////////////////////////////
			/// Summary: On every valid element do something
			template<typename Func>
			FilteredRange for_each( Func func ) const {
				auto result = copy_of_me( ).do_filter( );
				for( const auto current_value : result.m_value_refs ) {
					func( current_value );
				}
				return result;
			}

			//////////////////////////////////////////////////////////////////////////
			/// Summary: Copy all valid values to a std::vector
			std::vector<value_type> to_vector( ) const {
				auto result = std::vector<value_type>( );
				auto filt_iters = get_filtered_iterators( );
				for( auto it = filt_iters.first; it != filt_iters.second; ++it ) {
					result.push_back( *it );
				}
				return result;
			}

			//////////////////////////////////////////////////////////////////////////
			/// Summary: Copy all valid values to the provided range up to the
			/// smallest list.
			template<typename Iter>
			void copy_to( Iter first_inclusive, Iter last_exclusive ) const {
				auto filt_iter = get_filtered_iterators( );
				auto in_it = filt_iters.first;
				auto out_it = first_inclusive;
				while( in_it != filt_iter.second && out_it != last_exclusive ) {
					*out_it = *in_it;
					++in_it;
					++out_it;
				}
				return result;
			}

			//////////////////////////////////////////////////////////////////////////
			/// Summary: Append the values in the range to the current range
			template<typename Iter>
			FilteredRange append( Iter first_inclusive, Iter last_exclusive ) const {
				auto result = copy_of_me( );
				for( auto it = first_inclusive; it != last_exclusive; ++it ) {
					result.m_value_refs.push_back( std::reference_wrapper<value_type>( *it ) );
				}
				return result;
			}

			//////////////////////////////////////////////////////////////////////////
			/// Summary: Sort the elements in the range into ascending order.
			/// The elements are compared using operator< or the optional comp.
			/// Equivalent elements are not guaranteed to keep their original 
			/// relative order (see stable_sort)
			template<typename LessThanCompare = std::less<value_type>>
			FilteredRange sort( LessThanCompare comp = LessThanCompare( ) ) const {
				auto result = copy_of_me( ).do_filter( );
				std::sort( result.begin( ), result.end( ), comp );
				return result;
			}

			//////////////////////////////////////////////////////////////////////////
			/// Summary: Sort the elements in the range into ascending order.
			/// The elements are compared using operator< or the optional comp.
			/// Preserves the relative order of the elements with equivalent values
			template<typename EqualToCompare = std::less<value_type>>
			FilteredRange stable_sort( EqualToCompare comp = EqualToCompare( ) ) const {
				auto result = copy_of_me( ).do_filter( );
				std::stable_sort( result.begin( ), result.end( ), comp );
				return result;
			}

			//////////////////////////////////////////////////////////////////////////
			/// Summary: Removes all consecutive duplicate elements from the range
			template<typename EqualToCompare = std::equal_to<value_type>>
			FilteredRange unique( EqualToCompare comp = EqualToCompare( ) ) const {
				auto result = copy_of_me( ).do_filter( );
				auto new_last = std::unique( result.begin( ), result.end( ), comp );
				result.m_value_refs.erase( new_last, result.end( ) );
				return result;
			}

			//////////////////////////////////////////////////////////////////////////
			/// Summary: Sorts all elements and remove all consecutive duplicate
			/// elements
			template<typename EqualToCompare = std::less<value_type>, typename EqualCompare = std::equal_to<value_type>>
			FilteredRange sorted_unique( EqualToCompare scomp = EqualToCompare( ), EqualCompare ucomp = EqualCompare( ) ) const {
				auto result = sort( scomp );
				auto new_last = std::unique( result.begin( ), result.end( ), ucomp );
				result.m_value_refs.erase( new_last, result.end( ) );
				return result;
			}

			//////////////////////////////////////////////////////////////////////////
			/// Summary: Removes all duplicate elements from the range maintaining order.
			/// After the initial element, no other duplicates are included.
			template<typename EqualToCompare = std::equal_to<value_type>>
			FilteredRange stable_unique( EqualToCompare comp = EqualToCompare( ) ) const {
				auto result = std::vector<std::reference_wrapper<value_type>>( );
				for( auto& current_value : m_value_refs ) {
					if( value_included( current_value ) && result.end( ) == find( result.begin( ), result.end( ), current_value, comp ) ) {
						result.push_back( current_value );
					}
				}
				auto retval = FilteredRange( result, m_pred_include );
				return retval;
			}

			//////////////////////////////////////////////////////////////////////////
			/// Summary: Rearranges the elements of the range in such a way that 
			/// all the elements for which pred returns true precede all those for
			/// which it returns false. The relative ordering within each group is not 
			/// necessarily the same as before the call. See stable_partition for a
			/// function with a similar behavior but with stable ordering within each 
			/// group.
			template<typename UnaryPredicate>
				FilteredRange partiton( UnaryPredicate pred ) const {
				auto result = copy_of_me( ).do_filter( );
				std::partition( result.begin( ), result.end( ), pred );
				return result;
			}

			//////////////////////////////////////////////////////////////////////////
			/// Summary: Rearranges the elements of the range in such a way that 
			/// all the elements for which pred returns true precede all those for
			/// which it returns false.  The relative order of elements within each 
			/// group is preserved.
			template<typename UnaryPredicate>
			FilteredRange stable_partiton( UnaryPredicate pred ) const {
				auto result = copy_of_me( ).do_filter( );
				std::stable_partition( result.begin( ), result.end( ), pred );
				return result;
			}

			//////////////////////////////////////////////////////////////////////////
			/// Summary: Reverses the order of the elements in the range
			FilteredRange reverse( ) const {
				auto result = copy_of_me( ).do_filter( );
				std::reverse( result.begin( ), result.end( ), pred );
				return result;
			}

			//////////////////////////////////////////////////////////////////////////
			/// Summary: Rearranges the elements in the range randomly.  The 
			/// function swaps the value of each element with that of some other 
			/// randomly picked element.  The function gen determines which element is 
			/// picked in every case.
			template<typename RandomNumberGenerator>
			FilteredRange random_shuffle( RandomNumberGenerator rnd ) const {
				auto result = copy_of_me( ).do_filter( );
				std::random_shuffle( result.begin( ), result.end( ), rnd );
				return result;
			}

			//////////////////////////////////////////////////////////////////////////
			/// Summary: Rearranges the elements in the range randomly.  The 
			/// function swaps the value of each element with that of some other 
			/// randomly picked element.  The function uses some unspecified 
			/// source of randomness
			FilteredRange random_shuffle( ) const {
				auto result = copy_of_me( ).do_filter( );
				std::random_shuffle( result.begin( ), result.end( ) );
				return result;
			}

			//////////////////////////////////////////////////////////////////////////
			/// Summary: Replaces all elements equal to old_value with 
			/// new_value in the range.  By default ituses operator== to compare 
			/// the elements, otherwise it uses the given binary predicate comp. 
			template<typename EqualToCompare = std::equal_to<value_type>>
			FilteredRange replace( value_type old_value, value_type new_value, EqualToCompare comp = EqualToCompare( ) ) const {
				auto result = copy_of_me( ).do_filter( );
				std::replace( result.begin( ), result.end( ), old_value, new_value );
				return result;
			}

			//////////////////////////////////////////////////////////////////////////
			/// Summary: Replaces all elements satisfying pred with
			/// new_value in the range.
			template<typename UnaryPredicate>
			FilteredRange replace_if( UnaryPredicate pred, value_type new_value ) const {
				auto result = copy_of_me( ).do_filter( );
				std::replace_if( result.begin( ), result.end( ), pred, new_value );
				return result;
			}


			//////////////////////////////////////////////////////////////////////////
			/// Summary: Constructs a new range with the union of two 
			/// FilteredRange's formed by the elements that are present in either one, 
			/// or in both. 
			template<typename LessThanCompare = std::less<value_type>>
			FilteredRange set_union( FilteredRange other, LessThanCompare comp = LessThanCompare( ) ) const {
				auto result = copy_of_me( ).do_filter( ).sort( );
				other.do_filter( ).sort( );
				m_pred_include.insert( m_pred_include.end( ), other.m_pred_include.begin( ), other.m_pred_include.end( ) );	
				auto new_last = std::set_union( result.begin( ), result.end( ), other.begin( ), other.end( ), comp );
				result.m_value_refs.erase( new_last, result.end( ) );
				return result;
			}

			//////////////////////////////////////////////////////////////////////////
			/// Summary: Constructs a new range with the intersection of two 
			/// FilteredRange's formed by only by the elements that are present
			/// in both
			template<typename LessThanCompare = std::less<value_type>>
			FilteredRange set_intersection( FilteredRange other, LessThanCompare comp = LessThanCompare( ) ) const {
				auto result = copy_of_me( ).do_filter( ).sort( );
				other.do_filter( ).sort( );
				m_pred_include.insert( m_pred_include.end( ), other.m_pred_include.begin( ), other.m_pred_include.end( ) );
				auto new_last = std::set_intersection( result.begin( ), result.end( ), other.begin( ), other.end( ), comp );
				result.m_value_refs.erase( new_last, result.end( ) );
				return result;
			}

			//////////////////////////////////////////////////////////////////////////
			/// Summary: Constructs a new range with the difference of two 
			/// FilteredRange's formed by the elements that are present in the first 
			/// set, but not in the second one
			template<typename LessThanCompare = std::less<value_type>>
			FilteredRange set_difference( FilteredRange other, LessThanCompare comp = LessThanCompare( ) ) const {
				auto result = copy_of_me( ).do_filter( ).sort( );
				other.do_filter( ).sort( );
				m_pred_include.insert( m_pred_include.end( ), other.m_pred_include.begin( ), other.m_pred_include.end( ) );
				auto new_last = std::set_difference( result.begin( ), result.end( ), other.begin( ), other.end( ) );
				result.m_value_refs.erase( new_last, result.end( ) );
				return result;
			}

			//////////////////////////////////////////////////////////////////////////
			/// Summary: Constructs a new range with the difference of two 
			/// FilteredRange's formed by the elements that are present one of the
			/// sets but not the other
			FilteredRange set_symmetric_difference( FilteredRange other ) const {
				auto result = copy_of_me( ).do_filter( ).sort( );
				other.do_filter( ).sort( );
				m_pred_include.insert( m_pred_include.end( ), other.m_pred_include.begin( ), other.m_pred_include.end( ) );
				auto new_last = std::set_symmetric_difference( result.begin( ), result.end( ), other.begin( ), other.end( ) );
				result.m_value_refs.erase( new_last, result.end( ) );
				return result;
			}

			//////////////////////////////////////////////////////////////////////////
			/// Summary: Constructs a new range with the sorted values that have 
			/// duplicates
			template<typename EqualToCompare = std::equal_to<value_type>>
			FilteredRange duplicates( EqualToCompare comp = EqualToCompare( ) ) const {
				auto result = copy_of_me( ).do_filter( ).sort( );
				auto new_vals = std::vector<std::reference_wrapper<value_type>>( );
				auto it = result.begin( );
				while( it != result.end( ) ) {
					auto cur_val = *it;
					if( (it + 1) != result.end( ) && comp( cur_val, *(it + 1) ) ) {
						new_vals.push_back( cur_val );
					}
					++it;						
					while( it != result.end( ) && comp( cur_val, *it ) ) {
						++it;
					}
				}
				result.m_value_refs = new_vals;
				return result;
			}

			//////////////////////////////////////////////////////////////////////////
			/// Summary: Constructs a new range with the values that have 
			/// duplicates.  The first value is included, excluding all successive
			/// repeats
			template<typename EqualToCompare = std::equal_to<value_type>>
			FilteredRange stable_duplicates( EqualToCompare comp = EqualToCompare( ) ) const {
				auto valid_vals = duplicates( comp );
				auto result = copy_of_me( )
					.do_filter( )
					.where( [&valid_vals]( const value_type& value ) { return valid_vals.contains( value ); } )
					.stable_unique( )
					.do_filter( ).clear_where( );
				return result;
			}

			//////////////////////////////////////////////////////////////////////////
			/// Summary: Returns a boolean indicating if value is in the range
			template<typename EqualToCompare = std::equal_to<value_type>>
			bool contains( value_type value, EqualToCompare comp = EqualToCompare( ) ) const {
				auto result = copy_of_me( ).do_filter( );
				for( auto current_value : result.m_value_refs ) {
					if( comp( value, current_value ) ) {
						return true;
					}
				}
				return false;
			}

			//////////////////////////////////////////////////////////////////////////
			/// Summary: Returns a boolean indicating if the range is empty
			bool empty( ) const {
				return m_value_refs.empty( );
			}

			//////////////////////////////////////////////////////////////////////////
			/// Summary: Returns a FilteredRange with the result of calling func on it
			template<typename Func>
			FilteredRange call(Func func) const {
				auto result = copy_of_me( ).do_filter( );
				func( result );
				return result;
			}

		private:
			using iter_type = typename std::vector<std::reference_wrapper<value_type>>::iterator;

			using predicate_ref_type = std::function < bool( std::reference_wrapper<value_type> ) > ;
			using filtered_iterator = boost::filter_iterator < predicate_ref_type, iter_type > ;
			std::vector<std::reference_wrapper<value_type>> m_value_refs;
			std::vector<predicate_type> m_pred_include;

			iter_type begin( ) {
				return m_value_refs.begin( );
			}

			iter_type end( ) {
				return m_value_refs.end( );
			}

			FilteredRange copy_of_me( ) const {
				return FilteredRange( *this );
			}

			FilteredRange& do_filter( ) {
				if( !m_pred_include.empty( ) ) {
					auto new_last = std::remove_if( begin( ), end( ), [&]( const value_type& value ) { return !value_included( value ); } );
					m_value_refs.erase( new_last, end( ) );
				}
				return *this;
			}

			bool value_included( const value_type& value ) const {
				if( m_pred_include.empty( ) ) {
					return true;
				}
				for( auto& included : m_pred_include ) {
					if( !included( value ) ) {
						return false;
					}
				}
				return true;
			}

			template<typename EqualToCompare = std::equal_to<value_type>>
			static iter_type find( iter_type first_inclusive, iter_type last_exclusive, std::reference_wrapper<value_type> val, EqualToCompare comp ) {
				auto result = last_exclusive;
				for( auto it = first_inclusive; it != last_exclusive; ++it ) {
					if( comp( val, *it ) ) {
						result = it;
						break;
					}
				}
				return result;
			}

			FilteredRange( const std::vector<std::reference_wrapper<value_type>>& value_refs, std::vector<predicate_type> predicate_stack ): m_value_refs( value_refs ), m_pred_include( predicate_stack ) { }

			std::pair<filtered_iterator, filtered_iterator> get_filtered_iterators( ) {
				auto pred = [&]( std::reference_wrapper<value_type> value ) { return value_included( value.get( ) ); };

				auto filtered_begin = filtered_iterator( pred, begin( ), end( ) );
				auto filtered_end = filtered_iterator( pred, end( ), end( ) );
				return std::make_pair( filtered_begin, filtered_end );
			}
		};	// class FilteredRange

		template<typename Container>
		auto make_filtered_range( Container& container ) -> FilteredRange < typename std::iterator_traits<decltype(std::begin( container ))>::value_type > {
			return FilteredRange<typename std::iterator_traits<decltype(std::begin( container ))>::value_type>( std::begin( container ), std::end( container ) );
		}

		template<typename value_type>
		std::function <bool( value_type )> less_then( value_type value ) {
			return[value]( const value_type& test_val ) {
				return test_val < value;
			};
		}

		template<typename value_type>
		std::function <bool( value_type )> less_then_equal( value_type value ) {
			return[value]( const value_type& test_val ) {
				return test_val <= value;
			};
		}

		template<typename value_type>
		std::function <bool( value_type )> greater_then( value_type value ) {
			return[value]( const value_type& test_val ) {
				return test_val > value;
			};
		}

		template<typename value_type>
		std::function <bool( value_type )> greater_then_equal( value_type value ) {
			return[value]( const value_type& test_val ) {
				return test_val >= value;
			};
		}

		template<typename value_type>
		std::function <bool( value_type )> equals( value_type value ) {
			return[value]( const value_type& test_val ) {
				return test_val == value;
			};
		}

		template<typename value_type>
		std::function <bool( value_type )> logic_or( std::function <bool( value_type )> lhs, std::function <bool( value_type )> rhs ) {
			return[lhs, rhs]( const value_type& test_val ) {
				return lhs( test_val ) || rhs( test_val );
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
