#pragma once

#include <algorithm>
#include <boost/iterator/filter_iterator.hpp>
#include <functional>
#include <iterator>
#include <set>
#include <vector>

namespace daw {
	namespace range {
		template<typename value_type>
		class FilteredRange {
		public:
			using predicate_type = std::function < bool( value_type ) > ;

			template<typename Iter>
			FilteredRange( Iter first_inclusive, Iter last_exclusive ): m_value_refs( first_inclusive, last_exclusive ), m_pred_include( ) { }

			FilteredRange& operator=(FilteredRange rhs) {
				m_value_refs = std::move( rhs.m_value_refs );
				m_pred_include = std::move( rhs.m_pred_include );
			}

			FilteredRange( FilteredRange&& other ): m_value_refs( std::move( other.m_value_refs ) ), m_pred_include( std::move( other.m_pred_include ) ) { }
			FilteredRange( ) = delete;
			FilteredRange( const FilteredRange& ) = default;

			bool operator==(const FilteredRange&) const = delete;

			virtual ~FilteredRange( ) = default;

			FilteredRange where( predicate_type predicate ) const {
				auto result = copy_of_me( );
				result.m_pred_include.push_back( predicate );
				return result;
			}

			template<typename Func>
			FilteredRange for_each( Func func ) const {
				auto result = copy_of_me( );
				result.do_filter( );
				for( auto& current_value : result.m_value_refs ) {
					func( current_value );
				}
				return result;
			}

			std::vector<value_type> to_vector( ) const {
				auto result = std::vector<value_type>( );
				auto filt_iters = get_filtered_iterators( );
				for( auto& val : m_value_refs ) {
					result.push_back( val );
				}
				return result;
			}

			template<typename EqualToCompare = std::less<value_type>>
			FilteredRange sort( EqualToCompare comp = EqualToCompare( ) ) const {
				auto result = copy_of_me( );
				result.do_filter( );
				std::sort( result.m_value_refs.begin( ), result.m_value_refs.end( ), comp );
				return result;
			}

			template<typename EqualToCompare = std::equal_to<value_type>>
			FilteredRange unique( EqualToCompare comp = EqualToCompare( ) ) const {
				auto result = copy_of_me( );
				result.do_filter( );
				auto new_last = std::unique( result.m_value_refs.begin( ), result.m_value_refs.end( ), comp );
				result.m_value_refs.erase( new_last, result.m_value_refs.end( ) );
				return result;
			}

			template<typename EqualToCompare = std::less<value_type>, typename EqualCompare = std::equal_to<value_type>>
			FilteredRange sorted_unique( EqualToCompare scomp = EqualToCompare( ), EqualCompare ucomp = EqualCompare( ) ) const {
				auto result = sort( scomp );
				auto new_last = std::unique( result.begin( ), result.end( ), ucomp );
				result.m_value_refs.erase( new_last, result.end( ) );
				return result;
			}

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

			FilteredRange clear_where( ) const {
				auto result = copy_of_me( );
				result.do_filter( );
				result.m_pred_include.clear( );
				return result;
			}

			template<typename Iter>
			FilteredRange append( Iter first_inclusive, Iter last_exclusive ) const {
				auto result = copy_of_me( );
				for( auto it = first_inclusive; it != last_exclusive; ++it ) {
					result.m_value_refs.push_back( std::reference_wrapper<value_type>( *it ) );
				}
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

			void do_filter( ) {
				if( !m_pred_include.empty( ) ) {
					auto new_last = std::remove_if( begin( ), end( ), [&]( const value_type& value ) { return !value_included( value ); } );
					m_value_refs.erase( new_last, end( ) );
				}
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
	}	// namespace range	
}	// namespace daw
