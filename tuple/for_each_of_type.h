#ifndef FOR_EACH_OF_TYPE_HELPER
#define FOR_EACH_OF_TYPE_HELPER

#include "details/for_each_of_type.h"
#include "details/tuple_size.h"
#include "first_of_type.h"

namespace helpers
{

namespace tuple
{

template< typename Target, typename Functor, size_t start_index = 0, class T >
void for_each_of_type( T&& t, Functor&& f )
{
    constexpr size_t t_size{ details::tuple_size( std::forward< T >( t ) ) };
    constexpr size_t next_index{ first_of_type< Target, start_index >( std::forward< T >( t ) ) };
    details::_for_each_of_type< Target, Functor, next_index, t_size >( std::forward< T >( t ), std::forward< Functor >( f ) );
}

}// tuple

}// helpers

#endif
