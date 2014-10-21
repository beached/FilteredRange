#pragma once

#include "filtered_range_class.h"

namespace daw {
	namespace range {		
		template<typename value_type>
		class FilteredRangeGroup {
		public:
			FilteredRangeGroup& operator=(FilteredRangeGroup rhs);
			FilteredRangeGroup( FilteredRangeGroup&& other );
			FilteredRangeGroup( ) = default;
			FilteredRangeGroup( const FilteredRangeGroup& ) = default;
			bool operator==(const FilteredRangeGroup&) const;
			~FilteredRangeGroup( ) = default;
		private:
			std::vector<FilteredRange<value_type>> m_ranges;
		};	// class FilteredRangeGroup
	}	// namespace range	
}	// namespace daw
